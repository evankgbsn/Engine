#ifndef CIRCLE_H
#define CIRCLE_H

#include <glm/glm.hpp>

class LineSegment;
class Rectangle;
class OrientedRectangle;

class Circle
{
public:

	Circle(const glm::vec2& initialCenter, float initialRadius = 1.0f);

	~Circle();

	void SetCenter(const glm::vec2& newCenter);

	const glm::vec2& GetCenter() const;

	float GetRadius() const;

	bool PointIntersect(const glm::vec2& point) const;

	bool LineIntersect(const LineSegment& line) const;

	bool CircleIntersect(const Circle& circle) const;

	bool RectangleIntersect(const Rectangle& rectangle) const;

	bool OrientedRectangleIntersect(const OrientedRectangle& rectangle) const;

private:

	Circle() = delete;

	Circle(const Circle&) = delete;

	Circle& operator=(const Circle&) = delete;

	Circle(Circle&&) = delete;

	Circle& operator=(Circle&&) = delete;

	glm::vec2 center;

	float radius;
};

#endif // CIRCLE_H