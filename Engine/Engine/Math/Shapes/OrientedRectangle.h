#ifndef ORIENTEDRECTANGLE_H
#define ORIENTEDRECTANGLE_H

#include <glm/glm.hpp>

class LineSegment;

class OrientedRectangle
{
public:

	OrientedRectangle(const glm::vec2& initialPosition, const glm::vec2& initialHalfExtents, float initialRotation);

	~OrientedRectangle();

	bool PointIntersect(const glm::vec3& point) const;

	bool LineIntersect(const LineSegment& line) const;

private:

	OrientedRectangle() = delete;

	OrientedRectangle(const OrientedRectangle&) = delete;

	OrientedRectangle& operator=(const OrientedRectangle&) = delete;

	OrientedRectangle(OrientedRectangle&&) = delete;

	OrientedRectangle& operator=(OrientedRectangle&&) = delete;

	glm::vec2 position;

	glm::vec2 halfExtents;

	float rotation;

};

#endif // ORIENTEDRECTANGLE_H