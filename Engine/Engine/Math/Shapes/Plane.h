#ifndef PLANE_H
#define PLANE_H

#include <glm/glm.hpp>

class Plane
{

public:

	Plane(const glm::vec3& initialNormal, float initialDistanceFromOrigin);

	~Plane();

	const glm::vec3& GetNormal() const;

	float GetDistance() const;

	// Will return 0 if the point is on the plane.
	float PlaneEquation(const glm::vec3& point) const;

	bool PointOnPlane(const glm::vec3& point) const;

	glm::vec3 ClosestPoint(const glm::vec3& point) const;

private:

	Plane(const Plane&) = delete;

	Plane& operator=(const Plane&) = delete;

	Plane(Plane&&) = delete;

	Plane& operator=(Plane&&) = delete;

	glm::vec3 normal;

	float distance;
};

#endif // PLANE_H