#include "BoundingSphere.h"

#include "../Renderer/Model/ModelManager.h"
#include "../Renderer/Model/Model.h"
#include "../Renderer/Images/TextureManager.h"
#include "../Math/Math.h"
#include "../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "../Renderer/GraphicsObjects/ColoredStaticGraphicsObject.h"
#include "../Utils/Logger.h"

#include <glm/gtc/matrix_access.hpp>

BoundingSphere::BoundingSphere(std::function<void(Entity*)> callback, Entity* o, Model* m, const glm::mat4& initialTransform) :
	CollisionVolume(callback, owner),
	center(0.0f),
	originalCenter(0.0f),
	radius(0.0f),
	scalar(1.0f),
	boundingSphereVolume(nullptr),
	ownerModel(m),
	owner(o),
	initialized(std::atomic<bool>(false))
{
	GraphicsObjectManager::CreateColoredStaticGraphicsObject(ModelManager::GetModel("Sphere"), glm::vec4(0.0f, 0.5f, 0.5f, 1.0f), [this, initialTransform](ColoredStaticGraphicsObject* go) 
	{
			boundingSphereVolume = go;
			GraphicsObjectManager::WireFrame(go, ObjectTypes::GraphicsObjectType::ColoredStatic);
			Initialize(ownerModel, initialTransform);
			go->SetScale({ radius, radius, radius });
			go->SetTransform(initialTransform);
	});
}

BoundingSphere::~BoundingSphere()
{
}

const glm::vec3& BoundingSphere::GetCenter() const
{
	return center;
}

float BoundingSphere::GetRadius() const
{
	float returnVal = radius * scalar;
	return returnVal;
}

void BoundingSphere::ComputeData(Model* model, const glm::mat4& matrix)
{
	if (initialized.load() == true)
	{
		center = glm::vec3(matrix * glm::vec4(originalCenter, 1.0f));

		float scaleX = glm::length(matrix[0]);
		float scaleY = glm::length(matrix[1]);
		float scaleZ = glm::length(matrix[2]);

		// Bounding Spheres only support uniform scale at the moment.
		glm::mat4 rotation = glm::mat4(1.0f);
		rotation[0] = matrix[0] / scaleX;
		rotation[1] = matrix[1] / scaleY;
		rotation[2] = matrix[2] / scaleX;

		glm::mat4 scale = glm::mat4(1.0f);
		scale = glm::scale(scale, { radius, radius, radius });

		glm::mat4 scale0 = glm::mat4(1.0f);
		scale0 = glm::scale(scale0, { scaleX, scaleY, scaleZ });

		scale = scale0 * scale;

		glm::mat4 translation = glm::mat4(1.0f);
		translation = glm::translate(translation, center);

		scalar = scaleX;

		if (boundingSphereVolume != nullptr)
		{
			boundingSphereVolume->SetTransform(translation * rotation * scale);
		}
	}
}

bool BoundingSphere::Intersect(const CollisionVolume& other) const
{
	return other.Intersect(*this);
}

bool BoundingSphere::Intersect(const BoundingSphere& other) const
{
	return Math::Intersect(*this, other);
}

bool BoundingSphere::Intersect(const AxisAlignedBoundingBox& other) const
{
	return Math::Intersect(*this, other);
}

bool BoundingSphere::Intersect(const OrientedBoundingBox& other) const
{
	return Math::Intersect(other, *this);
}

void BoundingSphere::Initialize(Model* model, const glm::mat4&)
{
	const std::vector<Vertex>& verts = model->GetVertices();

	// Find min and max on all axes.
	glm::vec3 minX = verts[0].GetPosition();
	glm::vec3 minY = verts[0].GetPosition();
	glm::vec3 minZ = verts[0].GetPosition();

	glm::vec3 maxX = verts[0].GetPosition();
	glm::vec3 maxY = verts[0].GetPosition();
	glm::vec3 maxZ = verts[0].GetPosition();

	for (const Vertex& vert : verts)
	{
		glm::vec3 position = vert.GetPosition();

		if (position.x <= minX.x)
		{
			minX = position;
		}

		if (position.x >= maxX.x)
		{
			maxX = position;
		}

		if (position.y <= minY.y)
		{
			minY = position;
		}

		if (position.y >= maxY.y)
		{
			maxY = position;
		}

		if (position.z <= minZ.z)
		{
			minZ = position;
		}

		if (position.z >= maxZ.z)
		{
			maxZ = position;
		}
	}

	// Determine which axis has greatest range.
	int axis = 0; // x = 0, y = 1, z = 2
	float max = 0.0f;
	float cur = 0.0f;
	
	glm::vec3 xVec = minX - maxX;
	cur = glm::length(xVec);

	if (cur >= max)
	{
		axis = 0;
		max = cur; 
	}


	glm::vec3 yVec = minY - maxY;
	cur = glm::length(yVec);

	if (cur >= max)
	{
		axis = 1; 
		max = cur;
	}

	glm::vec3 zVec = minZ - maxZ;
	cur = glm::length(zVec);

	if (cur >= max)
	{
		axis = 2;
		max = cur;
	}

	// Determine center and radius.
	radius = max / 2.0f;
	switch (axis)
	{
	case 0:
		center = minX + glm::normalize(-xVec) * radius;
		break;
	case 1:
		center = minY + glm::normalize(-yVec) * radius;
		break;
	case 2:
		center = minZ + glm::normalize(-zVec) * radius;
		break;
	}

	// Adjust center and radius.
	for (const Vertex& vertex : verts)
	{
		glm::vec3 direction = vertex.GetPosition() - center;
		float dist = glm::length(direction);

		glm::vec3 normalizedDir = glm::normalize(direction);

		if (dist > radius)
		{
			dist -= radius;
			radius += dist;
			center += normalizedDir * dist;
		}
	}
	originalCenter = center;

	initialized.store(true);
}

void BoundingSphere::SetColor(const glm::vec4& newColor)
{
	if (boundingSphereVolume != nullptr)
	{
		boundingSphereVolume->SetColor(newColor);
	}
}

const glm::vec4& BoundingSphere::GetColor() const
{
	static glm::vec4 defaultReturn(1.0f);

	if (boundingSphereVolume != nullptr)
	{
		return boundingSphereVolume->GetColor();
	}

	return defaultReturn;
}