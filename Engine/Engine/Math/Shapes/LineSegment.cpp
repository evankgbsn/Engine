#include "LineSegment.h"

LineSegment::LineSegment(const glm::vec2& startPoint, const glm::vec2& endPoint) :
	points(std::vector<glm::vec2>({startPoint, endPoint}))
{
}

LineSegment::LineSegment(const std::vector<glm::vec2>& initialPoints) :
	points(initialPoints)
{
}

LineSegment::~LineSegment()
{

}

const glm::vec2& LineSegment::GetStart() const
{
	return points[0];
}

const glm::vec2& LineSegment::GetEnd() const
{
	return points[points.size() - 1];
}

void LineSegment::SetStart(const glm::vec2& newStart)
{
	points[0] = newStart;
}

void LineSegment::SetEnd(const glm::vec2& newEnd)
{
	points[points.size() - 1] = newEnd;
}

float LineSegment::GetLength() const
{
	// The points vector must be in order.
	return glm::length(points[0] - points[points.size() - 1]);
}

float LineSegment::GetLengthSq() const
{
	glm::vec2 lengthVector = points[0] - points[points.size() - 1];
	return glm::dot(lengthVector, lengthVector);
}

bool LineSegment::PointIntersect(const glm::vec2& point)
{
	// Find the slope.
	const glm::vec2& start = points[0];
	const glm::vec2& end = points[points.size() - 1];

	float dy = end.y - start.y;
	float dx = end.x - start.x;

	float m = dy / dx;

	// Find the Y-intercept.
	float b = start.y - m * start.x;

	return point.y == m * point.x + b;
}
