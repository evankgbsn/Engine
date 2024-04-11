#include "Rectangle.h"

#include "LineSegment.h"
#include "../SAT/Interval2D.h"
#include "OrientedRectangle.h"

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

void Rectangle::SetPosition(const glm::vec2& newPosition)
{
	origin = newPosition;
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
	const glm::vec2& lineStart = line.GetStart();
	const glm::vec2& lineEnd = line.GetEnd();

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

bool Rectangle::RectangleIntersect(const Rectangle& rectangle) const
{
	const glm::vec2 min = GetMin();
	const glm::vec2 max = GetMax();

	const glm::vec2 otherMin = rectangle.GetMin();
	const glm::vec2 otherMax = rectangle.GetMax();

	bool overlapOnXAxis = ((otherMin.x <= max.x) && (min.x <= otherMax.x));
	bool overlapOnYAxis = ((otherMin.y <= max.y) && (min.y <= otherMax.y));

	return overlapOnXAxis && overlapOnYAxis;
}

bool Rectangle::RectangleSATIntersect(const Rectangle& rectangle) const
{
	// Axesis to test
	glm::vec2 axesToTest[2] =
	{
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 1.0f)
	};

	for (unsigned int i = 0; i < 2; ++i)
	{
		// If the intervals don't overlap a separating axis has been found.
		if (!RectangleOverlapOnAxis(rectangle, axesToTest[i]))
			return false;
	}

	// All intervals overlap, separating axis not found, we have collision.
	return true;
}

bool Rectangle::OrientedRectangleIntersect(const OrientedRectangle& rectangle) const
{
	// Axesis to test
	glm::vec2 axesToTest[4] =
	{
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f),
		glm::vec2(0.0f)
	};

	glm::mat2 zRotation =
	{
		cosf(rectangle.GetRotation()), sinf(rectangle.GetRotation()),
		-sinf(rectangle.GetRotation()), cosf(rectangle.GetRotation())
	};

	// Construct separating axis three.
	glm::vec2 axis = glm::normalize(glm::vec2(rectangle.GetHalfExtents().x, 0.0f));
	axesToTest[2] = zRotation * axis;

	// Construct separating axis four.
	axis = glm::normalize(glm::vec2(0.0f, rectangle.GetHalfExtents().y));
	axesToTest[3] = zRotation * axis;

	for (unsigned int i = 0; i < 4; ++i)
	{
		// If the intervals don't overlap a separating axis has been found.
		if (!OrientedRectangleOverlapOnAxis(rectangle, axesToTest[i]))
			return false;
	}

	// All intervals overlap, separating axis not found, we have collision.
	return true;
}

bool Rectangle::RectangleOverlapOnAxis(const Rectangle& rectangle, const glm::vec2& axis) const
{
	Interval2D thisRectangle(*this, axis);
	Interval2D otherRectangle(rectangle, axis);
	return ((otherRectangle.GetMin() <= thisRectangle.GetMax()) && (thisRectangle.GetMin() <= otherRectangle.GetMax()));
}

bool Rectangle::OrientedRectangleOverlapOnAxis(const OrientedRectangle& rectangle, const glm::vec2& axis) const
{
	Interval2D thisRectangle(*this, axis);
	Interval2D otherRectangle(rectangle, axis);
	return ((otherRectangle.GetMin() <= thisRectangle.GetMax()) && (thisRectangle.GetMin() <= otherRectangle.GetMax()));
}
