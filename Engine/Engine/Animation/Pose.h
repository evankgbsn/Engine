#ifndef POSE_H
#define POSE_H

#include "../Math/Transform.h"

#include <vector>

class Pose
{

public:

	Pose();

	Pose(unsigned int numJoints);

	~Pose();

	Pose(const Pose&);

	Pose& operator=(const Pose&);

	Pose(Pose&&) = default;

	Pose& operator=(Pose&&) = default;

	bool operator==(const Pose& other);

	bool operator!=(const Pose& other);

	// This will retrieve the global transform of a joint.
	Math::Transform operator[](unsigned int index);

	Math::Transform GetGlobalTransform(unsigned int index) const;

	void Resize(unsigned int size);

	unsigned int Size() const;

	unsigned int GetParent(unsigned int index) const;

	void SetParent(unsigned int index, unsigned int parent);

	const Math::Transform& GetLocalTransform(unsigned int index) const;

	void SetLocalTransform(unsigned int index, const Math::Transform& transform);

	void GetJointMatrices(std::vector<glm::mat4>& outMatrices) const;

private:

	std::vector<Math::Transform> joints;

	std::vector<unsigned int> parents;

};


#endif // POSE_H