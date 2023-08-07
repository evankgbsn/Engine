#include "TransformComponent.h"

#include <functional>

TransformComponent::TransformComponent() :
	Component(Type::TRANSFORM),
	rotation(1.0f),
	transform(1.0f)
{
}

TransformComponent::~TransformComponent()
{
}

void TransformComponent::SetTranslation(const glm::vec3& newPosition)
{
	std::function<void()> function = [this, &newPosition]()
	{
		transform[3].x = newPosition.x;
		transform[3].y = newPosition.y;
		transform[3].z = newPosition.z;
		transform[3].w = 1.0f;
	};

	QueueCommand(function);
}

void TransformComponent::SetRotation(const glm::vec3& newRotation)
{
	std::function<void()> function = [this, &newRotation]()
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
	};

	QueueCommand(function);
}

void TransformComponent::SetScale(const glm::vec3& newScale)
{
	std::function<void()> function = [this, &newScale]()
	{
		glm::vec3 reverseScaleScalars = GetScale() / 1.0f;
		transform[0] *= reverseScaleScalars.x;
		transform[1] *= reverseScaleScalars.y;
		transform[2] *= reverseScaleScalars.z;

		transform[0] *= newScale.x;
		transform[1] *= newScale.y;
		transform[2] *= newScale.z;
	};

	QueueCommand(function);
}

void TransformComponent::Translate(const glm::vec3& translation)
{
	std::function<void()> function = [this, &translation]()
	{
		transform[3].x += translation.x;
		transform[3].y += translation.y;
		transform[3].z += translation.z;
	};
	
	QueueCommand(function);
}

void TransformComponent::Rotate(float rot, const glm::vec3& axis)
{
	std::function<void()> function = [this, rot, &axis]
	{
		rotation[0] = glm::normalize(transform[0]);
		rotation[1] = glm::normalize(transform[1]);
		rotation[2] = glm::normalize(transform[2]);

		transform = glm::rotate(transform, rot, axis);
	};

	QueueCommand(function);
}

void TransformComponent::Scale(const glm::vec3& scale)
{
	std::function<void()> function = [this, &scale]()
	{
		transform[0] *= scale.x;
		transform[1] *= scale.y;
		transform[2] *= scale.z;
	};

	QueueCommand(function);
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
