#ifndef LINESEGMENT3D_H
#define LINESEGMENT3D_H

#include <glm/glm.hpp>

class LineSegment3D
{
public:

	LineSegment3D(const glm::vec3& startPoint, const glm::vec3& endPoint);

	~LineSegment3D();

	float GetLength() const;

	float GetLengthSq() const;

	bool PointIntersect(const glm::vec3& point) const;

	glm::vec3 ClosestPoint(const glm::vec3& point) const;

private:

	LineSegment3D(const LineSegment3D&) = delete;

	LineSegment3D& operator=(const LineSegment3D&) = delete;

	LineSegment3D(LineSegment3D&&) = delete;

	LineSegment3D& operator=(LineSegment3D&&) = delete;

	glm::vec3 start;

	glm::vec3 end;
};

#endif // LINESEGMENT3D_H