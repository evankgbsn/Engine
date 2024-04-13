#ifndef ORIENTEDBOUNDINGBOX_H
#define ORIENTEDBOUNDINGBOX_H

#include <glm/glm.hpp>

#include <vector>

class Vertex;

class OrientedBoundingBox
{
public:

	OrientedBoundingBox(const glm::vec3& initialOrigin, const glm::vec3& initialSize, const glm::mat4& initialOrientation);

	OrientedBoundingBox(const std::vector<Vertex>& vertices, const glm::mat4& initialOrientation = glm::mat4(1.0f));

	~OrientedBoundingBox();

	const glm::vec3& GetOrigin() const;

	const glm::vec3& GetSize() const;

	const glm::mat4& GetOrientation() const;

	const glm::vec3& GetOffset() const;

	void SetOrigin(const glm::vec3& newOrigin);

	void SetSize(const glm::vec3& newSize);

	void SetOrientation(const glm::mat4& newOrientation);

	glm::vec3 ClosestPoint(const glm::vec3& point) const;

	bool PointIntersect(const glm::vec3& point) const;

	bool OrientedBoundingBoxIntersect(const OrientedBoundingBox& other) const;

	void SizeToMesh(const std::vector<Vertex>& vertices);

	void UpdateOrigin(const glm::mat4& mat);

private:

	OrientedBoundingBox(const OrientedBoundingBox&) = delete;

	OrientedBoundingBox& operator=(const OrientedBoundingBox&) = delete;

	OrientedBoundingBox(OrientedBoundingBox&&) = delete;

	OrientedBoundingBox& operator=(OrientedBoundingBox&&) = delete;

	bool OverlapOnAxis(const OrientedBoundingBox& other, glm::vec3& axis) const;

	glm::vec3 origin;

	glm::vec3 offset;

	glm::vec3 size;

	glm::mat4 orientation;

};

#endif // ORIENTEDBOUNDINGBOX_H