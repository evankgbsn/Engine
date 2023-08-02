#ifndef TransformComponent_H
#define TransformComponent_H

#include "Component.h"

#include <glm/gtc/matrix_transform.hpp>

class TransformComponent : public Component
{
public:

	TransformComponent();

	~TransformComponent();

	TransformComponent(const TransformComponent&) = default;

	TransformComponent& operator=(const TransformComponent&) = default;

	TransformComponent(TransformComponent&&) = default;

	TransformComponent& operator=(TransformComponent&&) = default;

	void SetTranslation(const glm::vec3& newPosition);

	void SetRotation(const glm::vec3& newRotation);

	void SetScale(const glm::vec3& newScale);

	void Translate(const glm::vec3& translation);

	void Rotate(float rotation, const glm::vec3& axis);

	void Scale(const glm::vec3& scale);

	const glm::mat4& GetTransform() const;

	const glm::vec4& GetTranslation() const;

	const glm::mat4& GetRotation() const;

	glm::vec3 GetScale() const;

protected:

private:

	glm::mat4 transform;

	glm::mat4 rotation;
};

#endif //TransformComponent_H