#include "TexturedAnimatedGraphicsObject.h"

#include "../../Animation/Clip.h"
#include "../../Animation/Armature.h"
#include "../Model/Model.h"
#include "../../Time/TimeManager.h"
#include "../../Utils/Logger.h"
#include "../Cameras/CameraManager.h"
#include "../Memory/UniformBuffer.h"
#include "../Images/Texture.h"

#include <vector>

TexturedAnimatedGraphicsObject::AnimationInstance::AnimationInstance(Clip* c, const Pose& p) :
	speed(1.0f),
	playback(0.0f),
	animatedPose(new Pose(p)),
	clip(c)
{
	unsigned int bakedPoseCount = clip->GetDuration() / 0.0041666;
	bakedPoses.resize(bakedPoseCount);
	for (std::vector<glm::mat4>& posePalette : bakedPoses)
	{
		posePalette.resize(animatedPose->Size());
	}

	for (unsigned int i = 0; i < bakedPoseCount; i++)
	{
		clip->Sample(*animatedPose, i * 0.004166f);
		animatedPose->GetJointMatrices(bakedPoses[i]);
	}
};

TexturedAnimatedGraphicsObject::AnimationInstance::~AnimationInstance()
{
	delete animatedPose;
};

void TexturedAnimatedGraphicsObject::AnimationInstance::Update(Armature* const armature, MVPUniformBuffer& ubo)
{
	if (armature == nullptr)
	{
		Logger::Log(std::string("Calling TexturedAnimatedGraphicsObject::AnimationInstance::Update() with an invlaid armature."), Logger::Category::Warning);
		return;
	}

	playback += TimeManager::DeltaTime();
	if (playback >= 0.004166)
	{
		for (unsigned int i = 0; i < bakedPoses[index].size(); ++i)
		{
			ubo.pose[i] = bakedPoses[index][i];
		}

		index++;
		if (index >= bakedPoses.size())
		{
			index = 0;
		}

		playback = 0;
	}

	const std::vector<glm::mat4>& invBindPose = armature->GetInvBindPose();
	for (unsigned int i = 0; i < invBindPose.size(); ++i)
	{
		ubo.invBindPose[i] = invBindPose[i];
	}
}

TexturedAnimatedGraphicsObject::TexturedAnimatedGraphicsObject(Model* const m, Texture* const tex) :
	GraphicsObject(m),
	animationInstance((model->GetAnimationClips().size() > 0) ? &model->GetAnimationClips()[0] : nullptr, model->GetArmature()->GetRestPose()),
	texture(tex)
{
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
	UniformBuffer* mvpUniformBuffer = new UniformBuffer(sizeof(mvp), 0);
	mvpUniformBuffer->PersistentMap();
	uniformBuffers.push_back(mvpUniformBuffer);
}

void TexturedAnimatedGraphicsObject::Update()
{
	mvp.model = glm::mat4(1.0f);

	const Camera& cam = CameraManager::GetCamera("MainCamera");

	mvp.view = cam.GetView();
	mvp.projection = cam.GetProjection();
	mvp.projection[1][1] *= -1;

	shouldUpdateAnim += TimeManager::DeltaTime();

	animationInstance.Update(model->GetArmature(), mvp);

	uniformBuffers[0]->SetData(&mvp);
}

bool TexturedAnimatedGraphicsObject::ToggleLoopAnimation()
{
	bool isLooping;
	animationInstance.clip->SetIsLooping(isLooping);
	animationInstance.clip->SetIsLooping(!isLooping);
	return isLooping;
}

void TexturedAnimatedGraphicsObject::SetAnimationSpeed(float animationSpeed)
{
	animationInstance.speed = animationSpeed;
}

void TexturedAnimatedGraphicsObject::SetClip(unsigned int clipIndex)
{
	std::vector<Clip>& clips = model->GetAnimationClips();

	if (clipIndex >= clips.size())
	{
		Logger::Log(std::string("Calling TexturedAnimatedGraphicsObject::SetClip with an invalid clip index."));
		return;
	}

	animationInstance.clip = &clips[clipIndex];
}
