#include "BoundingSphere.h"

#include "../Renderer/Model/ModelManager.h"
#include "../Renderer/Model/Model.h"
#include "../Renderer/Images/TextureManager.h"
#include "../Math/Math.h"
#include "../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "../Renderer/GraphicsObjects/ColoredStaticGraphicsObject.h"

#include <glm/gtc/matrix_access.hpp>

BoundingSphere::BoundingSphere(std::function<void(Entity*)> callback, Entity* o, Model* m, const glm::mat4& initialTransform) :
	CollisionVolume(callback, owner),
	center(),
	originalCenter(),
	radius(),
	boundingSphereVolume(nullptr),
	ownerModel(m),
	owner(o)
{

	GraphicsObjectManager::CreateColoredStaticGraphicsObject(ModelManager::GetModel("Sphere"), glm::vec4(0.0f, 0.5f, 0.5f, 1.0f), [this, initialTransform](ColoredStaticGraphicsObject* go) 
	{
			boundingSphereVolume = go;
			GraphicsObjectManager::WireFrame(go, ObjectTypes::GraphicsObjectType::ColoredStatic);
			Initialize(ownerModel, initialTransform);
			go->SetScale({ radius, radius, radius });
			go->SetTranslation(originalCenter);
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
	return radius;
}

void BoundingSphere::ComputeData(Model* model, const glm::mat4& matrix)
{
	center = glm::vec3(matrix * glm::vec4(originalCenter, 1.0f));

	float scaleX = glm::length(matrix[0]);
	float scaleY = glm::length(matrix[1]);
	float scaleZ = glm::length(matrix[2]);

	// Bounding Spheres only support uniform scale at the moment.
	glm::mat4 rotation(1.0f);
	rotation[0] = (matrix[0] / scaleX);
	rotation[1] = (matrix[1] / scaleY);
	rotation[2] = (matrix[2] / scaleX);

	glm::mat4 scale(1.0f);
	scale =  glm::scale(scale, { radius, radius, radius });

	glm::mat4 translation(1.0f);
	translation = glm::translate(translation, center);


	if(boundingSphereVolume != nullptr)
		boundingSphereVolume->SetTransform(translation * rotation * scale);
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
	const Vertex* verts = model->GetVertices().data();

	// Find min and max on all axes.
	const Vertex* minX = verts;
	const Vertex* minY = verts;
	const Vertex* minZ = verts;

	const Vertex* maxX = verts;
	const Vertex* maxY = verts;
	const Vertex* maxZ = verts;

	for (const Vertex* vert = verts; vert != verts + model->GetVertices().size() - 1; vert++)
	{
		if (vert->GetPosition().x <= minX->GetPosition().x) minX = vert;
		if (vert->GetPosition().x >= maxX->GetPosition().x) maxX = vert;

		if (vert->GetPosition().y <= minY->GetPosition().y) minY = vert;
		if (vert->GetPosition().y >= maxY->GetPosition().y) maxY = vert;

		if (vert->GetPosition().z <= minZ->GetPosition().z) minZ = vert;
		if (vert->GetPosition().z >= maxZ->GetPosition().z) maxZ = vert;
	}

	// Determine which axis has greatest range.
	int axis = 0; // x = 0, y = 1, z = 2
	float max = 0, cur = 0;

	if ((cur = glm::length((minX->GetPosition() - maxX->GetPosition()))) >= max) { axis = 0; max = cur; }
	if ((cur = glm::length((minY->GetPosition() - maxY->GetPosition()))) >= max) { axis = 1; max = cur; }
	if ((cur = glm::length((minZ->GetPosition() - maxZ->GetPosition()))) >= max) { axis = 2; max = cur; }

	// Determine center and radius.
	this->radius = max / 2;
	switch (axis)
	{
	case 0:
		this->center = minX->GetPosition() + glm::normalize((maxX->GetPosition() - minX->GetPosition())) * this->radius;
		break;
	case 1:
		this->center = minY->GetPosition() + glm::normalize((maxY->GetPosition() - minY->GetPosition())) * this->radius;
		break;
	case 2:
		this->center = minZ->GetPosition() + glm::normalize((maxZ->GetPosition() - minZ->GetPosition())) * this->radius;
		break;
	}

	// Adjust center and radius.
	for (const Vertex* vert = verts; vert != verts + model->GetVertices().size() - 1; vert++)
	{
		float dist;
		Vertex direction;
		if ((dist = glm::length(direction.GetPosition() = vert->GetPosition() - glm::vec3(this->center))) > radius)
		{
			radius += (dist -= radius);
			center += glm::normalize(direction.GetPosition()) * dist;
		}
	}
	originalCenter = center;

	if (boundingSphereVolume != nullptr)
	{
		boundingSphereVolume->Scale({ radius, radius, radius });
	}
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