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

const glm::vec2& OrientedRectangle::GetPosition() const
{
	return position;
}

float OrientedRectangle::GetRotation() const
{
	return rotation;
}

void OrientedRectangle::SetPosition(const glm::vec2& newPosition)
{
	position = newPosition;
}

void OrientedRectangle::SetRotation(float angle)
{
	rotation = angle;
}

const glm::vec2& OrientedRectangle::GetHalfExtents() const
{
	return halfExtents;
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
	const glm::vec2& lineStart = line.GetStart();
	const glm::vec2& lineEnd = line.GetEnd();

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

bool OrientedRectangle::OrientedRectangleIntersect(const OrientedRectangle& rectangle)
{
	Rectangle localRectangle(glm::vec2(0.0f), halfExtents * 2.0f);

	glm::vec2 r = rectangle.position - position;

	OrientedRectangle localRectangle2(rectangle.position, rectangle.halfExtents, rectangle.rotation);

	localRectangle2.rotation = localRectangle2.rotation - rotation;

	glm::mat2 zRotation =
	{
		cosf(rotation), sinf(rotation),
		-sinf(rotation), cosf(rotation)
	};

	r = zRotation * r;

	localRectangle2.position = r + halfExtents;

	return localRectangle.OrientedRectangleIntersect(localRectangle2);
}
