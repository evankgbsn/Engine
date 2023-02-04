#include "Pose.h"

Pose::Pose()
{
}

Pose::Pose(unsigned int numJoints)
{
	Resize(numJoints);
}

Pose::~Pose()
{
}

Pose::Pose(const Pose& other)
{
	*this = other;
}

Pose& Pose::operator=(const Pose& other)
{
	if (&other == this)
	{
		return *this;
	}
	
	if (parents.size() != other.parents.size())
	{
		parents.resize(other.parents.size());
	}

	if (joints.size() != other.joints.size())
	{
		joints.resize(other.joints.size());
	}

	if (parents.size() != 0)
	{
		memcpy(parents.data(), other.parents.data(), sizeof(unsigned int) * parents.size());
	}

	if (joints.size() != 0)
	{
		memcpy(joints.data(), other.joints.data(), sizeof(Math::Transform) * joints.size());
	}

	return *this;
}

bool Pose::operator==(const Pose& other)
{
	if (joints.size() != other.joints.size())
	{
		return false;
	}

	if (parents.size() != other.parents.size())
	{
		return false;
	}

	for (unsigned int i = 0; i < joints.size(); i++)
	{
		if (parents[i] != other.parents[i] || joints[i] != other.joints[i])
		{
			return false;
		}
	}

	return true;
}

bool Pose::operator!=(const Pose& other)
{
	return !(*this == other);
}

Math::Transform Pose::operator[](unsigned int index)
{
	return GetGlobalTransform(index);
}

Math::Transform Pose::GetGlobalTransform(unsigned int index) const
{
	Math::Transform result = joints[index];

	for (int parent = parents[index]; parent >= 0; parent = parents[parent])
	{
		result = Math::Transform::Combine(joints[parent], result);
	}

	return result;
}

void Pose::Resize(unsigned int size)
{
	joints.resize(size);
	parents.resize(size);
}

unsigned int Pose::Size() const
{
	return static_cast<unsigned int>(joints.size());
}

unsigned int Pose::GetParent(unsigned int index) const
{
	return parents[index];
}

void Pose::SetParent(unsigned int index, int parent)
{
	parents[index] = parent;
}

const Math::Transform& Pose::GetLocalTransform(unsigned int index) const
{
	return joints[index];
}

void Pose::SetLocalTransform(unsigned int index, const Math::Transform& transform)
{
	joints[index] = transform;
}

void Pose::GetJointMatrices(std::vector<glm::mat4>& outMatrices) const
{
	unsigned int size = Size();

	if (outMatrices.size() != size)
	{
		outMatrices.resize(Size());
	}

	for (unsigned int i = 0; i < size; i++)
	{
		outMatrices[i] = GetGlobalTransform(i).ToMat4();
	}
}
