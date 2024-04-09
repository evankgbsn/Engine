#include "Circle.h"

#include "LineSegment.h"

Circle::Circle(const glm::vec3& initialCenter, float initialRadius) :
	center(initialCenter),
	radius(initialRadius)
{
}

Circle::~Circle()
{
}

const glm::vec3& Circle::GetCenter() const
{
	return center;
}

float Circle::GetRadius() const
{
	return radius;
}

bool Circle::PointIntersect(const glm::vec3& point) const
{
	glm::vec3 lengthVector(point - center);
	float lengthToPointSq = glm::dot(lengthVector, lengthVector);
	return lengthToPointSq < radius*radius;
}

bool Circle::LineIntersect(const LineSegment& line) const
{
	const glm::vec3& lineStart = line.GetStart();
	const glm::vec3& lineEnd = line.GetEnd();
	glm::vec2 lineVec = lineEnd - lineStart;

	float t = glm::dot(glm::vec2(center) - glm::vec2(lineStart), lineVec) / glm::dot(lineVec, lineVec);
	if (t < 0.0f || t > 1.0f)
	{
		return false;
	}

	glm::vec2 closestPoint = glm::vec2(lineStart) + lineVec * t;

	LineSegment circleToClosest(center, glm::vec3(closestPoint, 0.0f));

	return circleToClosest.GetLengthSq() < radius * radius;
}

bool Circle::CircleIntersect(const Circle& circle) const
{
	const glm::vec2 lengthVec = circle.center - center;
	float sumRadii = circle.radius + radius;

	return glm::dot(lengthVec, lengthVec) < sumRadii * sumRadii;
}
