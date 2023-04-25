#include "DirectionalLight.h"

DirectionalLight::DirectionalLight() :
	direction(0.333f, -0.333f, 0.333f),
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
