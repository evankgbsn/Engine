#ifndef TransformComponent_H
#define TransformComponent_H

#include "Component.h"

#include <glm/gtc/matrix_transform.hpp>

class TransformComponent : public Component
{
public:

	TransformComponent();

	~TransformComponent();

	TransformComponent(const TransformComponent& other);

	TransformComponent& operator=(const TransformComponent&) = default;

	TransformComponent(TransformComponent&& other);

	TransformComponent& operator=(TransformComponent&&) = default;

	void SetTranslation(const glm::vec3& newPosition);

	void SetRotation(const glm::vec3& newRotation);

	void SetRotation(const glm::mat4& newRotation);

	void SetScale(const glm::vec3& newScale);

	void Translate(const glm::vec3& translation);

	void Rotate(float rotation, const glm::vec3& axis);

	void Scale(const glm::vec3& scale);

	glm::mat4 GetTransform() const;

	glm::vec4 GetTranslation() const;

	glm::mat4 GetRotation() const;

	glm::vec3 GetScale() const;

protected:

private:

	glm::mat4 transform;

	glm::mat4 rotation;
};

#endif //TransformComponent_H