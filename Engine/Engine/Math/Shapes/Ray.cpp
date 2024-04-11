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
