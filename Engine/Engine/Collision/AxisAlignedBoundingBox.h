#ifndef AXISALIGNEDBOUNDINGBOX_H
#define AXISALIGNEDBOUNDINGBOX_H

#include "CollisionVolume.h"

#include <glm/glm.hpp>

class Model;
class Entity;
class ColoredStaticGraphicsObject;

/**
	A class that represents the axis aligned bounding box collider.
*/
class AxisAlignedBoundingBox : public CollisionVolume
{


public:

	/**
		The constructor
	*/
	AxisAlignedBoundingBox(std::function<void(Entity*)> callback, Entity* owner, Model* ownerModel, const glm::mat4& initialTransform);

	/**
		The destructor
	*/
	~AxisAlignedBoundingBox();

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
		Set the min.
	*/
	void SetMin(glm::vec3 newMin);

	/**
		Set the max.
	*/
	void SetMax(glm::vec3 newMax);

	/**
		Conpute the data for the CollisionVolume.
	*/
	void ComputeData(class Model* model, const glm::mat4& matrix) override;

	/**
		Check if this collision volume intersects with another.
	*/
	bool Intersect(const CollisionVolume& other) const override;

	/**
		Check if this collision volume intersects with another.
	*/
	bool Intersect(const class BoundingSphere& other) const override;

	/**
		Check if this collision volume intersects with another.
	*/
	bool Intersect(const class AxisAlignedBoundingBox& other) const override;

	/**
		Check if this collision volume intersects with another.
	*/
	bool Intersect(const class OrientedBoundingBox& other) const override;

	/**
		Initialize the bounding box.
	*/
	void Initialize(class Model*, const glm::mat4&) override;

	/**
		Get the world matrix.
	*/
	glm::mat4& GetWorld() const;

private:

	AxisAlignedBoundingBox() = delete;

	/**
		Cannot copy.
	*/
	AxisAlignedBoundingBox(const AxisAlignedBoundingBox&) = delete;

	/**
		Cannot copy.
	*/
	AxisAlignedBoundingBox& operator=(const AxisAlignedBoundingBox&) = delete;

	/**
		Cannot move.
	*/
	AxisAlignedBoundingBox(AxisAlignedBoundingBox&&) = delete;

	/**
		Cannot move.
	*/
	AxisAlignedBoundingBox& operator=(AxisAlignedBoundingBox&&) = delete;

	void UpdateVisualization();


public:

private:

	/**
		The min point of the AABB
	*/
	glm::vec3 min;
	
	/**
		The max point for the AABB
	*/
	glm::vec3 max;

	/**
		The center point for the AABB.
	*/
	glm::vec3 center;

	/**
		The world matrix.
	*/
	glm::mat4 world;

	ColoredStaticGraphicsObject* boundingBoxVisualization;

	Entity* owner;

	Model* ownerModel;

};

#endif // AXISALIGNEDBOUNDINGBOX_H