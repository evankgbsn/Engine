#include "Sphere.h"

#include "AxisAlignedBoundingBox.h"
#include "OrientedBoundingBox.h"
#include "Plane.h"
#include "../../Renderer/Model/Vertex.h"

Sphere::Sphere(const glm::vec3& initialOrigin, float initialRadius) :
	origin(initialOrigin),
	radius(initialRadius)
{
}

Sphere::Sphere(const std::vector<Vertex>& verts)
{
	// Find min and max on all axes.
	glm::vec3 first = verts[0].GetPosition();
	glm::vec3 minX = first;
	glm::vec3 minY = first;
	glm::vec3 minZ = first;
	glm::vec3 maxX = first;
	glm::vec3 maxY = first;
	glm::vec3 maxZ = first;

	for (const Vertex& vert : verts)
	{
		const glm::vec3& pos = vert.GetPosition();
		if (pos.x <= minX.x) minX = pos;
		if (pos.x >= maxX.x) maxX = pos;

		if (pos.y <= minY.y) minY = pos;
		if (pos.y >= maxY.y) maxY = pos;

		if (pos.z <= minZ.z) minZ = pos;
		if (pos.z >= maxZ.z) maxZ = pos;
	}

	// Determine which axis has greatest range.
	int axis = 0; // x = 0, y = 1, z = 2
	float max = 0, cur = 0;

	if ((cur = glm::length((minX - maxX))) >= max) { axis = 0; max = cur; }
	if ((cur = glm::length((minY - maxY))) >= max) { axis = 1; max = cur; }
	if ((cur = glm::length((minZ - maxZ))) >= max) { axis = 2; max = cur; }

	// Determine center and radius.
	this->radius = max / 2;
	switch (axis)
	{
	case 0:
		origin = minX + glm::normalize((maxX - minX)) * radius;
		break;
	case 1:
		origin = minY + glm::normalize((maxY - minY)) * radius;
		break;
	case 2:
		origin = minZ + glm::normalize((maxZ - minZ)) * radius;
		break;
	}

	// Adjust center and radius.
	for (const Vertex& vert : verts)
	{
		const glm::vec3& pos = vert.GetPosition();
		float dist;
		glm::vec3 direction;
		if ((dist = glm::length(direction = pos - origin)) > radius)
		{
			radius += (dist -= radius);
			origin += glm::normalize(pos) * dist;
		}
	}
	offset = origin;
}

Sphere::~Sphere()
{
}

const glm::vec3& Sphere::GetOrigin() const
{
	return origin;
}

const glm::vec3& Sphere::GetOffset() const
{
	return offset;
}

float Sphere::GetRadius() const
{
	return radius;
}

void Sphere::SetOrigin(const glm::vec3& newOrigin)
{
	origin = newOrigin;
}

void Sphere::Transform(const glm::mat4& transform)
{
	origin = transform * glm::vec4(offset, 1.0f);
}

bool Sphere::PointIntersect(const glm::vec3& point) const
{
	glm::vec3 toPoint = point - origin;
	return glm::dot(toPoint, toPoint) <= radius * radius;
}

bool Sphere::SphereIntersect(const Sphere& other) const
{
	glm::vec3 distance = other.origin - origin;

	float lengthSq = glm::dot(distance, distance);

	float sumRadii = other.radius + radius;

	return lengthSq <= sumRadii * sumRadii;
}

bool Sphere::AxisAlignedBoundingBoxIntersect(const AxisAlignedBoundingBox& aabb) const
{
	glm::vec3 closest = aabb.ClosestPoint(origin);

	glm::vec3 distance = closest - origin;

	float lengthSq = glm::dot(distance, distance);

	return lengthSq <= radius * radius;
}

bool Sphere::OrientedBoundingBoxIntersect(const OrientedBoundingBox& obb) const
{
	glm::vec3 closest = obb.ClosestPoint(origin);

	glm::vec3 distance = closest - origin;

	float lengthSq = glm::dot(distance, distance);

	return lengthSq <= radius * radius;
}

bool Sphere::PlaneIntersect(const Plane& plane) const
{
	glm::vec3 closest = plane.ClosestPoint(origin);

	glm::vec3 distance = closest - origin;

	float lengthSq = glm::dot(distance, distance);

	return lengthSq <= radius * radius;
}

glm::vec3 Sphere::ClosestPoint(const glm::vec3& point) const
{
	glm::vec3 toPoint = glm::normalize(point - origin);
	return origin + toPoint * radius;
}
