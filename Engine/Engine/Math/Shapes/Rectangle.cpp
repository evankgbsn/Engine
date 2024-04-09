#include "Rectangle.h"

#include "LineSegment.h"

Rectangle Rectangle::FromMinMax(const glm::vec2& min, const glm::vec2& max)
{
	return Rectangle(min, max - min);
}

Rectangle::Rectangle(const glm::vec2& initialOrigin, const glm::vec2& initialSize) :
	origin(initialOrigin),
	size(initialSize)
{
}

Rectangle::~Rectangle()
{
}

const glm::vec2& Rectangle::GetMin() const
{
	const glm::vec2& p1 = origin;
	const glm::vec2 p2 = origin + size;

	return glm::vec2(fmin(p1.x, p2.x), fmin(p1.y, p2.y));
}

const glm::vec2& Rectangle::GetMax() const
{
	const glm::vec2& p1 = origin;
	const glm::vec2 p2 = origin + size;

	return glm::vec2(fmax(p1.x, p2.x), fmax(p1.y, p2.y));
}

bool Rectangle::PointIntersect(const glm::vec2& point) const
{
	glm::vec2 min = GetMin();
	glm::vec2 max = GetMax();

	return min.x <= point.x && min.y <= point.y && point.x <= max.x && point.y <= max.y;
}

bool Rectangle::LineIntersect(const LineSegment& line) const
{
	const glm::vec3& lineStart = line.GetStart();
	const glm::vec3& lineEnd = line.GetEnd();

	if (PointIntersect(lineStart) || PointIntersect(lineEnd))
		return true;

	glm::vec2 norm = glm::normalize(lineEnd - lineStart);
	norm.x = (norm.x != 0.0f) ? 1.0f / norm.x : 0;
	norm.y = (norm.y != 0.0f) ? 1.0f / norm.y : 0;

	glm::vec2 min = (GetMin() - glm::vec2(lineStart)) * norm;
	glm::vec2 max = (GetMax() - glm::vec2(lineStart)) * norm;

	float tMin = fmaxf(fminf(min.x, max.x), fmin(min.y, max.y));

	float tMax = fminf(fmaxf(min.x, max.x), fmax(min.y, max.y));

	if (tMax < 0 || tMin > tMax)
	{
		return false;
	}

	float t = (tMin < 0.0f) ? tMax : tMin;

	return t > 0.0f && t * t < line.GetLengthSq();
}
