#include "BakedAnimation.h"

#include "Armature.h"
#include "Clip.h"

BakedAnimation::BakedAnimation(Clip* c, Armature* const armature) :
	animatedPose(armature->GetRestPose()),
	clip(c),
	bakedPoses(std::vector<std::vector<glm::mat4>>())
{
	unsigned int bakedPoseCount = static_cast<unsigned int>(clip->GetDuration() / MAX_ANIMATION_FRAME_TIME);
	bakedPoses.resize(bakedPoseCount);
	for (std::vector<glm::mat4>& posePalette : bakedPoses)
	{
		posePalette.resize(animatedPose.Size());
	}

	for (unsigned int i = 0; i < bakedPoseCount; i++)
	{
		clip->Sample(animatedPose, i * MAX_ANIMATION_FRAME_TIME);
		animatedPose.GetJointMatrices(bakedPoses[i]);
	}
}

const std::vector<glm::mat4>& BakedAnimation::GetPoseAtIndex(unsigned int index) const
{
	static std::vector<glm::mat4> defaultReturn;

	if (index < bakedPoses.size())
	{
		return bakedPoses[index];
	}

	return defaultReturn;
}

unsigned int BakedAnimation::GetFrameCount() const
{
	return static_cast<unsigned int>(bakedPoses.size());
}

BakedAnimation::~BakedAnimation()
{
}