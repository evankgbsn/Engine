#include "DirectionalLight.h"

DirectionalLight::DirectionalLight() :
	direction(0.0f, -1.0f, 0.0f),
	color(0.333f, 0.333f, 0.333f)
{
}

DirectionalLight::DirectionalLight(const glm::vec3& dir, const glm::vec3& c) :
	direction(dir),
	color(c)
{
}

DirectionalLight::~DirectionalLight()
{
}

const glm::vec3 DirectionalLight::GetColor() const
{
	return color;
}

const glm::vec3 DirectionalLight::GetDirection() const
{
	return direction;
}

void DirectionalLight::SetDirection(const glm::vec3& newDirection)
{
	direction = newDirection;
}
