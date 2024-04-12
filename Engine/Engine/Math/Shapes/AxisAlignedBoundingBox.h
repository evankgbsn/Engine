#ifndef AXISALIGNEDBOUNDINGBOX_H
#define AXISALIGNEDBOUNDINGBOX_H

#include <glm/glm.hpp>

class Sphere;
class OrientedBoundingBox;
class Plane;

class AxisAlignedBoundingBox
{
public:

	AxisAlignedBoundingBox(const glm::vec3& initialOrigin, const glm::vec3& initialSize);

	~AxisAlignedBoundingBox();

	void FromMinAndMax(const glm::vec3& min, const glm::vec3& max);

	const glm::vec3& GetOrigin() const;

	const glm::vec3& GetSize() const;

	glm::vec3 GetMin() const;

	glm::vec3 GetMax() const;

	glm::vec3 ClosestPoint(const glm::vec3& point) const;

	bool PointIntersect(const glm::vec3& point) const;

	bool SphereIntersect(const Sphere& sphere);

	bool AxisAlignedBoundingBoxIntersect(const AxisAlignedBoundingBox& other) const;

	bool OrientedBoundingBoxIntersect(const OrientedBoundingBox& obb) const;

	bool PlaneIntersect(const Plane& plane) const;

private:

	bool OverlapOnAxis(const OrientedBoundingBox& obb, const glm::vec3& axis) const;

	AxisAlignedBoundingBox(const AxisAlignedBoundingBox&) = delete;

	AxisAlignedBoundingBox& operator=(const AxisAlignedBoundingBox&) = delete;
	
	AxisAlignedBoundingBox(AxisAlignedBoundingBox&&) = delete;

	AxisAlignedBoundingBox& operator=(AxisAlignedBoundingBox&&) = delete;

	glm::vec3 origin;

	glm::vec3 size;

};

#endif // AXISALIGNEDBOUNDINGBOX_H