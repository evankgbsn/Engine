#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include "Light.h"

#include <glm/glm.hpp>

class DirectionalLight : public Light
{
public:

	DirectionalLight();

	DirectionalLight(const glm::vec3& direction, const glm::vec3& color = glm::vec3(0.75f, 0.5f, 0.0f));

	~DirectionalLight();

	DirectionalLight(const DirectionalLight&) = delete;

	DirectionalLight& operator=(const DirectionalLight&) = delete;

	DirectionalLight(DirectionalLight&&) = delete;

	DirectionalLight& operator=(DirectionalLight&&) = delete;

	const glm::vec3 GetColor() const;

	const glm::vec3 GetDirection() const;

	void SetDirection(const glm::vec3& newDirection);

protected:

	glm::vec3 color;

	glm::vec3 direction;

private:

};

#endif // DIRECTIONALLIGHT_H