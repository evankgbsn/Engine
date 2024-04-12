#ifndef ORIENTEDBOUNDINGBOX_H
#define ORIENTEDBOUDNINGBOX_H

#include <glm/glm.hpp>

class OrientedBoundingBox
{
public:

	OrientedBoundingBox(const glm::vec3& initialOrigin, const glm::vec3& initialSize, const glm::mat4& initialOrientation);

	~OrientedBoundingBox();

	const glm::vec3& GetOrigin() const;

	const glm::vec3& GetSize() const;

	const glm::mat4& GetOrientation() const;

	void SetOrigin(const glm::vec3& newOrigin);

	void SetSize(const glm::vec3& newSize);

	void SetOrientation(const glm::mat4& newOrientation);

	glm::vec3 ClosestPoint(const glm::vec3& point) const;

	bool PointIntersect(const glm::vec3& point) const;

	bool OrientedBoundingBoxIntersect(const OrientedBoundingBox& other) const;

private:

	OrientedBoundingBox(const OrientedBoundingBox&) = delete;

	OrientedBoundingBox& operator=(const OrientedBoundingBox&) = delete;

	OrientedBoundingBox(OrientedBoundingBox&&) = delete;

	OrientedBoundingBox& operator=(OrientedBoundingBox&&) = delete;

	bool OverlapOnAxis(const OrientedBoundingBox& other, glm::vec3& axis) const;

	glm::vec3 origin;

	glm::vec3 size;

	glm::mat4 orientation;

};

#endif // ORIENTEDBOUNDINGBOX_H