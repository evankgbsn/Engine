#ifndef LINESEGMENT_H
#define LINESEGMENT_H

#include <glm/glm.hpp>

#include <vector>

class LineSegment
{
public:

	LineSegment(const glm::vec2& startPoint, const glm::vec2& endPoint);

	LineSegment(const std::vector<glm::vec2>& points);

	~LineSegment();

	const glm::vec2& GetStart() const;

	const glm::vec2& GetEnd() const;

	void SetStart(const glm::vec2& newStart);

	void SetEnd(const glm::vec2& newEnd);

	float GetLength() const;

	float GetLengthSq() const;

	bool PointIntersect(const glm::vec2& point);

private:

	LineSegment() = delete;

	LineSegment(const LineSegment&) = delete;

	LineSegment& operator=(const LineSegment&) = delete;

	LineSegment(LineSegment&&) = delete;

	LineSegment& operator=(LineSegment&&) = delete;

	std::vector<glm::vec2> points;
};

#endif // LINESEGMENT_H