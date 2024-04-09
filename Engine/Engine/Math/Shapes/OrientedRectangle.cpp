#include "OrientedRectangle.h"

#include "Rectangle.h"
#include "LineSegment.h"

OrientedRectangle::OrientedRectangle(const glm::vec2& initialPosition, const glm::vec2& initialHalfExtents, float initialRotation) :
	position(initialPosition),
	halfExtents(initialHalfExtents),
	rotation(initialRotation)
{
}

OrientedRectangle::~OrientedRectangle()
{
}

bool OrientedRectangle::PointIntersect(const glm::vec3& point) const
{
	glm::vec2 rotVec = glm::vec2(point) - position;
	
	glm::mat2 zRotation =
	{
		cosf(rotation), sinf(rotation),
		-sinf(rotation), cosf(rotation)
	};

	rotVec = zRotation * rotVec;

	Rectangle localRectangle(glm::vec2(0.0f), halfExtents * 2.0f);

	glm::vec2 localPoint = rotVec + halfExtents;
	
	return localRectangle.PointIntersect(localPoint);
}

bool OrientedRectangle::LineIntersect(const LineSegment& line) const
{
	const glm::vec3& lineStart = line.GetStart();
	const glm::vec3& lineEnd = line.GetEnd();

	glm::mat2 zRotation =
	{
		cosf(rotation), sinf(rotation),
		-sinf(rotation), cosf(rotation)
	};

	LineSegment localLine(glm::vec3(0.0f), glm::vec3(0.0f));

	glm::vec2 rotVec = glm::vec2(lineStart) - position;
	rotVec = zRotation * rotVec;

	localLine.SetStart(glm::vec3(rotVec + halfExtents, 0.0f));

	rotVec = glm::vec2(lineEnd) - position;
	rotVec = zRotation * rotVec;

	localLine.SetEnd(glm::vec3(rotVec + halfExtents, 0.0f));

	Rectangle localRectangle(glm::vec2(), halfExtents * 2.0f);

	return localRectangle.LineIntersect(localLine);
}
