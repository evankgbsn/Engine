#include "Armature.h"

Armature::Armature()
{
}

Armature::Armature(const Pose& restPose, const Pose& bindPose, const std::vector<std::string>& jointNames)
{
	Set(restPose, bindPose, jointNames);
}

Armature::~Armature()
{
}

void Armature::Set(const Pose& rest, const Pose& bind, const std::vector<std::string>& names)
{
	restPose = rest;
	bindPose = bind;
	jointNames = names;
	UpdateInvBindPose();
}

const Pose& Armature::GetRestPose() const
{
	return restPose;
}

const Pose& Armature::GetBindPose() const
{
	return bindPose;
}

const std::vector<glm::mat4>& Armature::GetInvBindPose() const
{
	return invBindPose;
}

const std::vector<std::string>& Armature::GetJointNames() const
{
	return jointNames;
}

const std::string& Armature::GetJointName(unsigned int index) const
{
	return jointNames[index];
}

void Armature::UpdateInvBindPose()
{
	unsigned int size = bindPose.Size();
	invBindPose.resize(size);

	for (unsigned int i = 0; i < size; ++i)
	{
		Math::Transform world = bindPose.GetGlobalTransform(i);
		invBindPose[i] = glm::inverse(world.ToMat4());
	}
}
