#include "Sphere.h"

Sphere::Sphere(const glm::vec3& initialOrigin, float initialRadius) :
	origin(initialOrigin),
	radius(initialRadius)
{
}

Sphere::~Sphere()
{
}

const glm::vec3& Sphere::GetOrigin() const
{
	return origin;
}

float Sphere::GetRadius() const
{
	return radius;
}

bool Sphere::PointIntersect(const glm::vec3& point) const
{
	glm::vec3 toPoint = point - origin;
	return glm::dot(toPoint, toPoint) <= radius * radius;
}

const glm::vec3& Sphere::ClosestPoint(const glm::vec3& point) const
{
	glm::vec3 toPoint = glm::normalize(point - origin);
	return origin + toPoint * radius;
}
