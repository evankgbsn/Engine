#ifndef BAKEDANIMATION_H
#define BAKEDANIMATION_H

#define MAX_ANIMATION_FRAME_TIME 0.0041666f
#define ANIMATION_PLAYBACK_FRAME_TIME 0.0041666f

#include "Pose.h"

#include <vector>
#include <glm/glm.hpp>

class Clip;
class Pose;
class Armature;

class BakedAnimation
{
public:

	BakedAnimation() = delete;

	BakedAnimation(Clip* c, Armature* const armature);

	~BakedAnimation();

	BakedAnimation(const BakedAnimation&) = default;

	BakedAnimation& operator=(const BakedAnimation&) = default;

	BakedAnimation(BakedAnimation&&) = default;

	BakedAnimation& operator=(BakedAnimation&&) = default;

	const std::vector<glm::mat4>& GetPoseAtIndex(unsigned int index) const;

	unsigned int GetFrameCount() const;

private:

	std::vector<std::vector<glm::mat4>> bakedPoses;
	Pose animatedPose;
	Clip* clip;

};


#endif // BAKEDANIMATION_H