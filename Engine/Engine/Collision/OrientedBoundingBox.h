#ifndef OrientedBoundingBox_h
#define OrientedBoundingBox_h
#include "CollisionVolume.h"

/**
	A class that represents the axis aligned bounding box collider.
*/
class OrientedBoundingBox : public CollisionVolume
{


public:

	/**
		The constructor
	*/
	OrientedBoundingBox(std::function<void(Entity*)> callback, Entity* owner, const std::vector<Vertex>& vertices, const glm::mat4& initialTransform);

	/**
		The destructor
	*/
	~OrientedBoundingBox();

	/**
		Get the center of the bounding box.
	*/
	const glm::vec3& GetCenter() const final;

	/**
		Get the min point.
	*/
	const glm::vec3& GetMin() const;

	/**
		Get the max point.
	*/
	const glm::vec3& GetMax() const;

	/**
		Get the world matrix.
	*/
	const glm::mat4& GetWorld() const;

	/**
		Conpute the data for the CollisionVolume.
	*/
	void ComputeData(const std::vector<Vertex>& vertices, const glm::mat4& matrix) final;

	/**
		Check if this collision volume intersects with another.
	*/
	bool Intersect(const CollisionVolume& other) const final;

	/**
		Check if this collision volume intersects with another.
	*/
	bool Intersect(const class BoundingSphere& other) const final;

	/**
		Check if this collision volume intersects with another.
	*/
	bool Intersect(const class OrientedBoundingBox& other) const final;

	/**
		Check if this collision volume intersects with another.
	*/
	bool Intersect(const class AxisAlignedBoundingBox& other) const final;

	/**
		Initialize the bounding box.
	*/
	void Initialize(const std::vector<Vertex>& vertices, const glm::mat4&) final;

	/**
		Get the scale. Used in visualization.
	*/
	const glm::mat4& GetWorldWithScale() const;


private:

	OrientedBoundingBox() = delete;

	/**
		Cannot copy.
	*/
	OrientedBoundingBox(const OrientedBoundingBox&) = delete;

	/**
		Cannot copy.
	*/
	OrientedBoundingBox& operator=(const OrientedBoundingBox&) = delete;

	/**
		Cannot move.
	*/
	OrientedBoundingBox(OrientedBoundingBox&&) = delete;

	/**
		Cannot move.
	*/
	OrientedBoundingBox& operator=(OrientedBoundingBox&&) = delete;


public:

private:

	/**
		The min point of the AABB
	*/
	glm::vec3 min;
	glm::vec3 worldMin;

	/**
		The max point for the AABB
	*/
	glm::vec3 max;
	glm::vec3 worldMax;

	/**
		The center point for the AABB.
	*/
	glm::vec3 center;

	/**
		The world matrix for the obb.
	*/
	glm::mat4 worldMat;
	glm::mat4 worldWithScale;
};






























#endif