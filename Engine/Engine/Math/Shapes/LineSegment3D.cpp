#include "LineSegment3D.h"

LineSegment3D::LineSegment3D(const glm::vec3& startPoint, const glm::vec3& endPoint) :
	start(startPoint),
	end(endPoint)
{
}

LineSegment3D::~LineSegment3D()
{
}

float LineSegment3D::GetLength() const
{
	return glm::length(end - start);
}

float LineSegment3D::GetLengthSq() const
{
	glm::vec2 vect = end - start;
	return glm::dot(vect, vect);
}

bool LineSegment3D::PointIntersect(const glm::vec3& point) const
{
	glm::vec3 closest = ClosestPoint(point);
	float distance = glm::dot(point - closest, point - closest);

	return distance <= 0.00005f && distance >= -0.00005f;
}

glm::vec3 LineSegment3D::ClosestPoint(const glm::vec3& point) const
{
	glm::vec3 vec = end - start;
	float t = glm::dot(point - start, vec) / glm::dot(vec, vec);
	t = fmaxf(t, 0.0f);
	t = fminf(t, 1.0f);
	return start + vec * t;
}
