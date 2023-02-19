#include "BakedAnimation.h"

#include "Armature.h"
#include "Clip.h"

BakedAnimation::BakedAnimation(Clip* c, Armature* const armature) :
	animatedPose(armature->GetRestPose()),
	clip(c),
	bakedPoses(std::vector<std::vector<glm::mat4>>())
{
	unsigned int bakedPoseCount = clip->GetDuration() / 0.0041666;
	bakedPoses.resize(bakedPoseCount);
	for (std::vector<glm::mat4>& posePalette : bakedPoses)
	{
		posePalette.resize(animatedPose.Size());
	}

	for (unsigned int i = 0; i < bakedPoseCount; i++)
	{
		clip->Sample(animatedPose, i * 0.004166f);
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
	return bakedPoses.size();
}

BakedAnimation::~BakedAnimation()
{
}