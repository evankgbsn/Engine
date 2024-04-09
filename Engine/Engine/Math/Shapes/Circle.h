#ifndef CIRCLE_H
#define CIRCLE_H

#include <glm/glm.hpp>

class LineSegment;

class Circle
{
public:

	Circle(const glm::vec3& initialCenter, float initialRadius = 1.0f);

	~Circle();

	const glm::vec3& GetCenter() const;

	float GetRadius() const;

	bool PointIntersect(const glm::vec3& point) const;

	bool LineIntersect(const LineSegment& line) const;

	bool CircleIntersect(const Circle& circle) const;

private:

	Circle() = delete;

	Circle(const Circle&) = delete;

	Circle& operator=(const Circle&) = delete;

	Circle(Circle&&) = delete;

	Circle& operator=(Circle&&) = delete;

	glm::vec3 center;

	float radius;
};

#endif // CIRCLE_H