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
	animation()
{
	type = ObjectTypes::GraphicsObjectType::AnimatedTextured;
	mvp.model = glm::mat4(1.0f);
	shaderName = "TexturedAnimated";
	InitializeDescriptorSets();

	dirLight = LightManager::GetDirectionalLight("MainDirLight");

	animation = new Animation(model->GetBakedAnimation(0));

	for (unsigned int i = 0; i < model->GetArmature()->GetInvBindPose().size(); i++)
	{
		anim.invBindPose[i] = model->GetArmature()->GetInvBindPose()[i];
	}
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
	Camera* cam = &CameraManager::GetActiveCamera();

	mvp.model = translation * rotation * scale;

	mvp.view = cam->GetView();
	mvp.projection = cam->GetProjection();
	mvp.projection[1][1] *= -1;

	animation->Update(anim.pose);

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

const glm::mat4* const TexturedAnimatedGraphicsObject::GetAnimPoseArray()
{
	return anim.pose;
}
