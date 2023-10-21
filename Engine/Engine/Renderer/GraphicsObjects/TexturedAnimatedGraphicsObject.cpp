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
	animation(new Animation(model->GetBakedAnimation(0)))
{
	mvp.model = glm::mat4(1.0f);
	shaderName = "TexturedAnimated";
	InitializeDescriptorSets();
}

TexturedAnimatedGraphicsObject::~TexturedAnimatedGraphicsObject()
{
	delete animation;
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

	UniformBuffer* animationUniformBuffer = new UniformBuffer(sizeof(anim), 2);
	animationUniformBuffer->PersistentMap();
	uniformBuffers.push_back(animationUniformBuffer);

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
	animation->SetSpeed(animationSpeed);
}

void TexturedAnimatedGraphicsObject::SetClip(unsigned int clipIndex)
{
	Animation* oldAnimation = animation;
	animation = new Animation(model->GetBakedAnimation(clipIndex));
	delete oldAnimation;
}

void TexturedAnimatedGraphicsObject::Translate(const glm::vec3& translation)
{
	mvp.model = glm::translate(mvp.model, translation);
}

void TexturedAnimatedGraphicsObject::Scale(const glm::vec3& scale)
{
}

void TexturedAnimatedGraphicsObject::Rotate(float angle, const glm::vec3& axis)
{
}

glm::vec3 TexturedAnimatedGraphicsObject::GetTranslation() const
{
	return mvp.model[3];
}

glm::vec3 TexturedAnimatedGraphicsObject::GetScale() const
{
	return glm::vec3(glm::length(mvp.model[0]), glm::length(mvp.model[1]), glm::length(mvp.model[2]));
}

glm::mat4 TexturedAnimatedGraphicsObject::GetRotation() const
{
	return glm::mat4(
		glm::normalize(mvp.model[0]),
		glm::normalize(mvp.model[1]),
		glm::normalize(mvp.model[2]),
		glm::vec4(0.0f)
	);
}
