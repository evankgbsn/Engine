#include "Ray.h"

Ray::Ray(const glm::vec3& initialOrigin, const glm::vec3& initialDirection) :
	origin(initialOrigin),
	direction(initialDirection)
{
}

Ray::~Ray()
{
}

const glm::vec3& Ray::GetOrigin() const
{
	return origin;
}

const glm::vec3& Ray::GetDirection() const
{
	return direction;
}

bool Ray::PointIntersect(const glm::vec3& point) const
{
	if (point == origin)
		return true;

	glm::vec3 norm = glm::normalize(point - origin);

	float diff = glm::dot(direction, norm);

	return diff >= 0.99992 && diff <= 1.00002;
}

glm::vec3 Ray::ClosestPoint(const glm::vec3& point) const
{
	float t = glm::dot(point - origin, direction);

	t = fmaxf(t, 0.0f);

	return origin + direction * t;
}
