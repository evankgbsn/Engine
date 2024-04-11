#include "Circle.h"

#include "LineSegment.h"
#include "Rectangle.h"
#include "OrientedRectangle.h"

Circle::Circle(const glm::vec2& initialCenter, float initialRadius) :
	center(initialCenter),
	radius(initialRadius)
{
}

Circle::~Circle()
{
}

void Circle::SetCenter(const glm::vec2& newCenter)
{
	center = newCenter;
}

const glm::vec2& Circle::GetCenter() const
{
	return center;
}

float Circle::GetRadius() const
{
	return radius;
}

bool Circle::PointIntersect(const glm::vec2& point) const
{
	glm::vec2 lengthVector(point - center);
	float lengthToPointSq = glm::dot(lengthVector, lengthVector);
	return lengthToPointSq < radius*radius;
}

bool Circle::LineIntersect(const LineSegment& line) const
{
	const glm::vec2& lineStart = line.GetStart();
	const glm::vec2& lineEnd = line.GetEnd();
	glm::vec2 lineVec = lineEnd - lineStart;

	float t = glm::dot(glm::vec2(center) - glm::vec2(lineStart), lineVec) / glm::dot(lineVec, lineVec);
	if (t < 0.0f || t > 1.0f)
	{
		return false;
	}

	glm::vec2 closestPoint = glm::vec2(lineStart) + lineVec * t;

	LineSegment circleToClosest(center, closestPoint);

	return circleToClosest.GetLengthSq() < radius * radius;
}

bool Circle::CircleIntersect(const Circle& circle) const
{
	const glm::vec2 lengthVec = circle.center - center;
	float sumRadii = circle.radius + radius;

	return glm::dot(lengthVec, lengthVec) < sumRadii * sumRadii;
}

bool Circle::RectangleIntersect(const Rectangle& rectangle) const
{
	const glm::vec2 min = rectangle.GetMin();
	const glm::vec2 max = rectangle.GetMax();

	glm::vec2 closestPoint = center;

	if (closestPoint.x < min.x)
	{
		closestPoint.x = min.x;
	}
	else if (closestPoint.x > max.x)
	{
		closestPoint.x = max.x;
	}

	if (closestPoint.y < min.y)
	{
		closestPoint.y = min.y;
	}
	else if(closestPoint.y > max.y)
	{
		closestPoint.y = max.y;
	}

	LineSegment localLine(center, closestPoint);

	return localLine.GetLengthSq() <= radius * radius ;
}

bool Circle::OrientedRectangleIntersect(const OrientedRectangle& rectangle) const
{
	const glm::vec2& rectanglePosition = rectangle.GetPosition();
	glm::vec2 circleToRectangle = center - rectanglePosition;

	glm::mat2 zRotation =
	{
		cosf(rectangle.GetRotation()), sinf(rectangle.GetRotation()),
		-sinf(rectangle.GetRotation()), cosf(rectangle.GetRotation())
	};

	circleToRectangle = zRotation * circleToRectangle;

	Circle localCircle(circleToRectangle + rectangle.GetHalfExtents(), radius);
	Rectangle localRectangle(glm::vec2(0.0f), rectangle.GetHalfExtents() * 2.0f);

	return localCircle.RectangleIntersect(localRectangle);
}
