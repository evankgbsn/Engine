#include "TransformComponent.h"

TransformComponent::TransformComponent() :
	rotation(1.0f),
	transform(1.0f)
{
}

TransformComponent::~TransformComponent()
{
}

void TransformComponent::SetTranslation(const glm::vec3& newPosition)
{
	transform[3].x = newPosition.x;
	transform[3].y = newPosition.y;
	transform[3].z = newPosition.z;
	transform[3].w = 1.0f;
}

void TransformComponent::SetRotation(const glm::vec3& newRotation)
{
	// Something might be wrong... Fix later.
	glm::vec3 scale = GetScale();
	glm::vec3 translation = GetTranslation();
	glm::mat4 newUpdatedTransform(1.0f);

	newUpdatedTransform[0].x = scale.x;
	newUpdatedTransform[1].y = scale.y;
	newUpdatedTransform[2].z = scale.z;
	newUpdatedTransform[3] = GetTranslation();

	transform = glm::rotate(newUpdatedTransform, 1.0f, newRotation);
}

void TransformComponent::SetScale(const glm::vec3& newScale)
{
	glm::vec3 reverseScaleScalars = GetScale() / 1.0f;
	transform[0] *= reverseScaleScalars.x;
	transform[1] *= reverseScaleScalars.y;
	transform[2] *= reverseScaleScalars.z;

	transform[0] *= newScale.x;
	transform[1] *= newScale.y;
	transform[2] *= newScale.z;
}

void TransformComponent::Translate(const glm::vec3& translation)
{
	transform[3].x += translation.x;
	transform[3].y += translation.y;
	transform[3].z += translation.z;
}

void TransformComponent::Rotate(float rot, const glm::vec3& axis)
{
	rotation[0] = glm::normalize(transform[0]);
	rotation[1] = glm::normalize(transform[1]);
	rotation[2] = glm::normalize(transform[2]);

	transform = glm::rotate(transform, rot, axis); 
}

void TransformComponent::Scale(const glm::vec3& scale)
{
	transform[0] *= scale.x;
	transform[1] *= scale.y;
	transform[2] *= scale.z;
}

const glm::mat4& TransformComponent::GetTransform() const
{
	return transform;
}

const glm::vec4& TransformComponent::GetTranslation() const
{
	return transform[3];
}

const glm::mat4& TransformComponent::GetRotation() const
{
	return rotation;
}

glm::vec3 TransformComponent::GetScale() const
{
	return glm::vec3(glm::length(transform[0]), glm::length(transform[1]), glm::length(transform[2]));
}
