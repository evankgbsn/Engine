#include "AxisAlignedBoundingBox.h"

#include "../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "../Renderer/Model/ModelManager.h"
#include "../Renderer/Model/Model.h"
#include "../Renderer/Images/TextureManager.h"
#include "../Math/Math.h"

AxisAlignedBoundingBox::AxisAlignedBoundingBox(std::function<void(Entity*)> callback, Entity* owner) :
	CollisionVolume(callback, owner),
	min(glm::vec3()),
	max(glm::vec3()),
	center(glm::vec3()),
	world(glm::mat4(1.0f)),
	boundingBoxVisualization(nullptr)
{

	GraphicsObjectManager::CreateTexturedStaticGraphicsObject(ModelManager::GetModel("Cube"), TextureManager::GetTexture("DefaultFontTexture"), [](GraphicsObject* go) {});

}

AxisAlignedBoundingBox::~AxisAlignedBoundingBox()
{
}

const glm::vec3& AxisAlignedBoundingBox::GetCenter() const
{
	return center;
}

const glm::vec3& AxisAlignedBoundingBox::GetMin() const
{
	return min;
}

const glm::vec3& AxisAlignedBoundingBox::GetMax() const
{
	return max;
}

void AxisAlignedBoundingBox::SetMin(glm::vec3 newMin)
{
	min = newMin;
	center = min + ((max - min) / 2.f);

	glm::mat4 newWorld(1.0f);
	newWorld[0] *= abs(min.x - max.x) / 2;
	newWorld[1] *= abs(min.y - max.y) / 2;
	newWorld[2] *= abs(min.z - max.z) / 2;
	newWorld[3] = glm::vec4(center, 1.0f);

	world = newWorld;
}

void AxisAlignedBoundingBox::SetMax(glm::vec3 newMax)
{
	max = newMax;
	center = min + ((max - min) / 2.f);

	glm::mat4 newWorld(1.0f);
	newWorld[0] *= abs(min.x - max.x) / 2;
	newWorld[1] *= abs(min.y - max.y) / 2;
	newWorld[2] *= abs(min.z - max.z) / 2;
	newWorld[3] = glm::vec4(center, 1.0f);

	world = newWorld;
}

void AxisAlignedBoundingBox::ComputeData(Model* model, const glm::mat4& mat)
{
	const std::vector<Vertex>& verts = model->GetVertices();

	min = mat * glm::vec4(verts[0].GetPosition(), 1.0f);
	max = mat * glm::vec4(verts[0].GetPosition(), 1.0f);

	for (const Vertex& x : verts)
	{
		glm::vec3 point = mat * glm::vec4(x.GetPosition(), 1.0f);

		if (point.x >= max.x) max.x = point.x;
		if (point.x <= min.x) min.x = point.x;

		if (point.y >= max.y) max.y = point.y;
		if (point.y <= min.y) min.y = point.y;

		if (point.z >= max.z) max.z = point.z;
		if (point.z <= min.z) min.z = point.z;
	}

	center = min + ((max - min) / 2.f);

	glm::mat4 newWorld(1.0f);
	newWorld[0] *= abs(min.x - max.x) / 2;
	newWorld[1] *= abs(min.y - max.y) / 2;
	newWorld[2] *= abs(min.z - max.z) / 2;
	newWorld[3] = glm::vec4(center, 1.0f);

	world = newWorld;
}

bool AxisAlignedBoundingBox::Intersect(const CollisionVolume& other) const
{
	return other.Intersect(*this);
}

bool AxisAlignedBoundingBox::Intersect(const BoundingSphere& other) const
{
	return Math::Intersect(other, *this);
}

bool AxisAlignedBoundingBox::Intersect(const AxisAlignedBoundingBox& other) const
{
	return Math::Intersect(*this, other);
}

bool AxisAlignedBoundingBox::Intersect(const OrientedBoundingBox& other) const
{
	return Math::Intersect(other, *this);
}

void AxisAlignedBoundingBox::Initialize(Model* model, const glm::mat4& mat)
{
	
	const std::vector<Vertex>& verts = model->GetVertices();

	min = mat * glm::vec4(verts[0].GetPosition(), 1.0f);
	max = mat * glm::vec4(verts[0].GetPosition(), 1.0f);

	for (const Vertex& x : verts)
	{
		glm::vec3 point = mat * glm::vec4(x.GetPosition(), 1.0f);

		if (point.x >= max.x) max.x = point.x;
		if (point.x <= min.x) min.x = point.x;

		if (point.y >= max.y) max.y = point.y;
		if (point.y <= min.y) min.y = point.y;

		if (point.z >= max.z) max.z = point.z;
		if (point.z <= min.z) min.z = point.z;
	}

	center = min + ((max - min) / 2.f);

	glm::mat4 newWorld(1.0f);
	newWorld[0] *= abs(min.x - max.x) / 2;
	newWorld[1] *= abs(min.y - max.y) / 2;
	newWorld[2] *= abs(min.z - max.z) / 2;
	newWorld[3] = glm::vec4(center, 1.0f);

	world = newWorld;

}

glm::mat4& AxisAlignedBoundingBox::GetWorld() const
{
	return const_cast<glm::mat4&>(world);
}
