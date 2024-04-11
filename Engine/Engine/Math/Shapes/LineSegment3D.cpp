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
