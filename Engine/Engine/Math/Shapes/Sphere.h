#ifndef SPHERE_H
#define SPHERE_H

#include <glm/glm.hpp>

class AxisAlignedBoundingBox;
class OrientedBoundingBox;
class Plane;

class Sphere
{

public:

	Sphere(const glm::vec3& initialOrigin, float initialRadius);

	~Sphere();

	const glm::vec3& GetOrigin() const;

	float GetRadius() const;

	bool PointIntersect(const glm::vec3& point) const;

	bool SphereIntersect(const Sphere& other) const;

	bool AxisAlignedBoundingBoxIntersect(const AxisAlignedBoundingBox& aabb) const;

	bool OrientedBoundingBoxIntersect(const OrientedBoundingBox& obb) const;

	bool PlaneIntersect(const Plane& plane) const;

	glm::vec3 ClosestPoint(const glm::vec3& point) const;

private:

	Sphere(const Sphere&) = delete;

	Sphere& operator=(const Sphere&) = delete;
	
	Sphere(Sphere&&) = delete;

	Sphere& operator=(Sphere&&) = delete;

	glm::vec3 origin;

	float radius;
};

#endif // SPHERE_H