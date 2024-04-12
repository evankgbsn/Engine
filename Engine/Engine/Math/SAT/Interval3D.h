#ifndef INTERVAL3D_H
#define INTERVAL3D_H

#include <glm/glm.hpp>

class AxisAlignedBoundingBox;
class OrientedBoundingBox;

class Interval3D
{

public:

	Interval3D(const AxisAlignedBoundingBox& aabb, const glm::vec3& axis);

	Interval3D(const OrientedBoundingBox& obb, const glm::vec3& axis);

	~Interval3D();

	float GetMin() const;

	float GetMax() const;

private:

	Interval3D(const Interval3D&) = delete;

	Interval3D& operator=(const Interval3D&) = delete;

	Interval3D(Interval3D&&) = delete;

	Interval3D& operator=(Interval3D&&) = delete;

	float min;

	float max;
};

#endif // INTERVAL3D_H