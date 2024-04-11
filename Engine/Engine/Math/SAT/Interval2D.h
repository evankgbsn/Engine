#ifndef INTERVAL2D_H
#define INTERVAL2D_H

#include <glm/glm.hpp>

class Rectangle;
class OrientedRectangle;

class Interval2D
{
public:

	Interval2D(const Rectangle& rectangle, const glm::vec2& axis);

	Interval2D(const OrientedRectangle& rectangle, const glm::vec2& axis);

	~Interval2D();

	float GetMin() const;

	float GetMax() const;

private:

	Interval2D() = delete;

	Interval2D(const Interval2D&) = delete;

	Interval2D& operator=(const Interval2D&) = delete;

	Interval2D(Interval2D&&) = delete;

	Interval2D& operator=(Interval2D&&) = delete;

	float min;

	float max;

};

#endif // INTERVAL2D_H