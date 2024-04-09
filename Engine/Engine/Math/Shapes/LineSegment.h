#ifndef LINESEGMENT_H
#define LINESEGMENT_H

#include <glm/glm.hpp>

#include <vector>

class LineSegment
{
public:

	LineSegment(const glm::vec3& startPoint, const glm::vec3& endPoint);

	LineSegment(const std::vector<glm::vec3>& points);

	~LineSegment();

	const glm::vec3& GetStart() const;

	const glm::vec3& GetEnd() const;

	void SetStart(const glm::vec3& newStart);

	void SetEnd(const glm::vec3& newEnd);

	float GetLength() const;

	float GetLengthSq() const;

	bool PointIntersect(const glm::vec3& point);

private:

	LineSegment() = delete;

	LineSegment(const LineSegment&) = delete;

	LineSegment& operator=(const LineSegment&) = delete;

	LineSegment(LineSegment&&) = delete;

	LineSegment& operator=(LineSegment&&) = delete;

	std::vector<glm::vec3> points;
};

#endif // LINESEGMENT_H