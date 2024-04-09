#include "ColoredAnimatedGraphicsObject.h"

#include "../Cameras/CameraManager.h"
#include "../Lights/LightManager.h"
#include "../Lights/DirectionalLight.h"
#include "../Memory/UniformBuffer.h"
#include "../../Animation/Animation.h"
#include "../../Animation/Armature.h"
#include "../Model/ModelManager.h"
#include "../Model/Model.h"

ColoredAnimatedGraphicsObject::ColoredAnimatedGraphicsObject(const Model* const model, const glm::vec4& c) :
	GraphicsObject(model),
	color({c}),
	animation()
{
	type = ObjectTypes::GraphicsObjectType::AnimatedTextured;
	mvp.model = glm::mat4(1.0f);
	shaderName = "ColoredAnimated";
	InitializeDescriptorSets();

	dirLight = LightManager::GetDirectionalLight("MainDirLight");

	animation = new Animation(model->GetBakedAnimation(0));

	for (unsigned int i = 0; i < model->GetArmature()->GetInvBindPose().size(); i++)
	{
		anim.invBindPose[i] = model->GetArmature()->GetInvBindPose()[i];
	}
}

ColoredAnimatedGraphicsObject::~ColoredAnimatedGraphicsObject()
{
}

void ColoredAnimatedGraphicsObject::Update()
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
	uniformBuffers[3]->SetData(&color);
}

void ColoredAnimatedGraphicsObject::SetAnimationSpeed(float animationSpeed)
{
	animation->SetSpeed(animationSpeed);
}

void ColoredAnimatedGraphicsObject::SetClip(unsigned int clipIndex)
{
	Animation* oldAnimation = animation;
	animation = new Animation(model->GetBakedAnimation(clipIndex));
	delete oldAnimation;
}

const glm::mat4* const ColoredAnimatedGraphicsObject::GetAnimPoseArray()
{
	return anim.pose;
}

const glm::mat4* const ColoredAnimatedGraphicsObject::GetAnimInvBindPoseArray()
{
	return anim.invBindPose;
}

void ColoredAnimatedGraphicsObject::SetColor(const glm::vec4& newColor)
{
	color.color = newColor;
}

const glm::vec4& ColoredAnimatedGraphicsObject::GetColor() const
{
	return color.color;
}

void ColoredAnimatedGraphicsObject::CreateTextures()
{
	
}

void ColoredAnimatedGraphicsObject::CreateUniformBuffers()
{
	UniformBuffer* mvpUniformBuffer = new UniformBuffer(sizeof(mvp), 0);
	mvpUniformBuffer->PersistentMap();
	uniformBuffers.push_back(mvpUniformBuffer);

	UniformBuffer* animationUniformBuffer = new UniformBuffer(sizeof(anim), 2);
	animationUniformBuffer->PersistentMap();
	uniformBuffers.push_back(animationUniformBuffer);

	UniformBuffer* lightsUniformBuffer = new UniformBuffer(sizeof(light), 3);
	lightsUniformBuffer->PersistentMap();
	uniformBuffers.push_back(lightsUniformBuffer);

	UniformBuffer* colorUniformBuffer = new UniformBuffer(sizeof(color), 4);
	colorUniformBuffer->PersistentMap();
	uniformBuffers.push_back(colorUniformBuffer);
}
