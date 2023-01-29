#ifndef ARMATURE_H
#define ARMATURE_H

#include "Pose.h"

#include <vector>
#include <string>

class Armature
{
public:

	Armature();

	Armature(const Pose& restPose, const Pose& bindPose, const std::vector<std::string>& jointNames);

	~Armature();

	Armature(const Armature&) = default;

	Armature& operator=(const Armature&) = default;

	Armature(Armature&&) = default;

	Armature& operator=(Armature&&) = default;

	void Set(const Pose& restPose, const Pose& bindPose, const std::vector<std::string>& jointNames);

	const Pose& GetRestPose() const;

	const Pose& GetBindPose() const;

	const std::vector<glm::mat4>& GetInvBindPose() const;

	const std::vector<std::string>& GetJointNames() const;

	const std::string& GetJointName(unsigned int index) const;

private:

	void UpdateInvBindPose();

	Pose restPose;

	Pose bindPose;

	std::vector<glm::mat4> invBindPose;

	std::vector<std::string> jointNames;

};

#endif // ARMATURE_H