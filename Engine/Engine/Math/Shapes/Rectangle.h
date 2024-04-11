#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <glm/glm.hpp>

class LineSegment;
class OrientedRectangle;

class Rectangle
{
public:

	static Rectangle FromMinMax(const glm::vec2& min, const glm::vec2& max);

	Rectangle(const glm::vec2& initialOrigin, const glm::vec2& initialSize);

	~Rectangle();

	void SetPosition(const glm::vec2& newPosition);

	const glm::vec2& GetMin() const;

	const glm::vec2& GetMax() const;

	bool PointIntersect(const glm::vec2& point) const;

	bool LineIntersect(const LineSegment& lineSegment) const;

	bool RectangleIntersect(const Rectangle& rectangle) const;

	bool RectangleSATIntersect(const Rectangle& rectangle) const;

	bool OrientedRectangleIntersect(const OrientedRectangle& rectangle) const;

private:

	bool RectangleOverlapOnAxis(const Rectangle& rectangle, const glm::vec2& axis) const;

	bool OrientedRectangleOverlapOnAxis(const OrientedRectangle& rectangle, const glm::vec2& axis) const;

	Rectangle() = delete;

	glm::vec2 origin;

	glm::vec2 size;

};

#endif // RECTANGLE_H