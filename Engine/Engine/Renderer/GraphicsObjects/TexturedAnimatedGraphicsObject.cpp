#include "TexturedAnimatedGraphicsObject.h"

#include "../../Animation/Clip.h"
#include "../../Animation/Armature.h"
#include "../Model/Model.h"
#include "../../Time/TimeManager.h"
#include "../../Utils/Logger.h"
#include "../Cameras/CameraManager.h"
#include "../Memory/UniformBuffer.h"
#include "../Images/Texture.h"
#include "../../Animation/Animation.h"
#include "../Lights/LightManager.h"
#include "../Lights/DirectionalLight.h"

#include <vector>

TexturedAnimatedGraphicsObject::TexturedAnimatedGraphicsObject(Model* const m, Texture* const tex) :
	GraphicsObject(m),
	texture(tex),
	animation(new Animation(model->GetBakedAnimation(1)))
{
	mvp.model = glm::mat4(1.0f);
	shaderName = "TexturedAnimated";
	InitializeDescriptorSets();
}

TexturedAnimatedGraphicsObject::~TexturedAnimatedGraphicsObject()
{
}

void TexturedAnimatedGraphicsObject::CreateTextures()
{
	textures.push_back(texture);
	texture->SetBinding(1U);
}

void TexturedAnimatedGraphicsObject::CreateUniformBuffers()
{
	// The binding for the texture sampler is 1.

	UniformBuffer* mvpUniformBuffer = new UniformBuffer(sizeof(mvp), 0);
	mvpUniformBuffer->PersistentMap();
	uniformBuffers.push_back(mvpUniformBuffer);

	UniformBuffer* animUniformBuffer = new UniformBuffer(sizeof(anim), 2);
	animUniformBuffer->PersistentMap();
	uniformBuffers.push_back(animUniformBuffer);

	UniformBuffer* lightsUniformBuffer = new UniformBuffer(sizeof(light), 3);
	lightsUniformBuffer->PersistentMap();
	uniformBuffers.push_back(lightsUniformBuffer);
}

void TexturedAnimatedGraphicsObject::Update()
{
	const Camera& cam = CameraManager::GetCamera("MainCamera");

	mvp.view = cam.GetView();
	mvp.projection = cam.GetProjection();
	mvp.projection[1][1] *= -1;

	for (unsigned int i = 0; i < model->GetArmature()->GetInvBindPose().size(); i++)
	{
		anim.invBindPose[i] = model->GetArmature()->GetInvBindPose()[i];
	}

	animation->Update(anim.pose);

	DirectionalLight* dirLight = LightManager::GetDirectionalLight("MainDirLight");

	light.color = glm::vec4(dirLight->GetColor(), 1.0f);
	light.direction = glm::vec4(dirLight->GetDirection(), 0.0f);
	light.ambient = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);

	uniformBuffers[0]->SetData(&mvp);
	uniformBuffers[1]->SetData(&anim);
	uniformBuffers[2]->SetData(&light);
}

bool TexturedAnimatedGraphicsObject::ToggleLoopAnimation()
{
	return false;
}

void TexturedAnimatedGraphicsObject::SetAnimationSpeed(float animationSpeed)
{
	
}

void TexturedAnimatedGraphicsObject::SetClip(unsigned int clipIndex)
{

}

void TexturedAnimatedGraphicsObject::Translate(const glm::vec3& translation)
{
	mvp.model = glm::translate(mvp.model, translation);
}

void TexturedAnimatedGraphicsObject::Scale(const glm::vec3&)
{
}

void TexturedAnimatedGraphicsObject::Rotate(const glm::vec3&)
{
}

extern glm::vec3 vec3DefaultReturn;
extern glm::mat4 mat4DefaultReturn;

const glm::vec3& TexturedAnimatedGraphicsObject::GetTranslation() const
{
	return vec3DefaultReturn;
}

const glm::vec3& TexturedAnimatedGraphicsObject::GetScale() const
{
	return vec3DefaultReturn;
}

const glm::mat4& TexturedAnimatedGraphicsObject::GetRotation() const
{
	return mat4DefaultReturn;
}
