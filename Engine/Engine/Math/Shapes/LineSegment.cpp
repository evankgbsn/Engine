#include "LineSegment.h"

LineSegment::LineSegment(const glm::vec3& startPoint, const glm::vec3& endPoint) :
	points(std::vector<glm::vec3>({startPoint, endPoint}))
{
}

LineSegment::LineSegment(const std::vector<glm::vec3>& initialPoints) :
	points(initialPoints)
{
}

LineSegment::~LineSegment()
{

}

const glm::vec3& LineSegment::GetStart() const
{
	return points[0];
}

const glm::vec3& LineSegment::GetEnd() const
{
	return points[points.size() - 1];
}

void LineSegment::SetStart(const glm::vec3& newStart)
{
	points[0] = newStart;
}

void LineSegment::SetEnd(const glm::vec3& newEnd)
{
	points[points.size() - 1] = newEnd;
}

float LineSegment::GetLength() const
{
	// The points vector must be in order.
	return glm::length(points[1] - points[points.size() - 1]);
}

float LineSegment::GetLengthSq() const
{
	glm::vec3 lengthVector = points[1] - points[points.size() - 1];
	return glm::dot(lengthVector, lengthVector);
}

bool LineSegment::PointIntersect(const glm::vec3& point)
{
	// Find the slope.
	const glm::vec3& start = points[0];
	const glm::vec3& end = points[points.size() - 1];

	float dy = end.y - start.y;
	float dx = end.x - start.x;

	float m = dy / dx;

	// Find the Y-intercept.
	float b = start.y - m * start.x;

	return point.y == m * point.x + b;
}
