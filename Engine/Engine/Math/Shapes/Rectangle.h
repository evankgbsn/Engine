#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <glm/glm.hpp>

class LineSegment;

class Rectangle
{
public:

	static Rectangle FromMinMax(const glm::vec2& min, const glm::vec2& max);

	Rectangle(const glm::vec2& initialOrigin, const glm::vec2& initialSize);

	~Rectangle();

	const glm::vec2& GetMin() const;

	const glm::vec2& GetMax() const;

	bool PointIntersect(const glm::vec2& point) const;

	bool LineIntersect(const LineSegment&) const;

private:

	Rectangle() = delete;

	glm::vec2 origin;

	glm::vec2 size;

};

#endif // RECTANGLE_H