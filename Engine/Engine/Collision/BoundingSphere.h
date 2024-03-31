#ifndef BoundingSphere_h
#define BoundingSphere_h

#include "CollisionVolume.h"

class OrientedBoundingBox;
class ColoredStaticGraphicsObject;

/**
	A bounding sphere collision volume...
*/
class BoundingSphere : public CollisionVolume
{

public:

	/**
		The constructor.
	*/
	BoundingSphere(std::function<void(Entity*)> callback, Entity* owner, Model* ownerModel, const glm::mat4& initialTransform);

	/**
		The destructor.
	*/
	~BoundingSphere();

	/**
		Get the center of the bounding sphere.
	*/
	const glm::vec3& GetCenter() const final;

	/**
		Get the radius of the boundingsphere.
	*/
	float GetRadius() const;

	/**
		Conpute the data for this bounding sphere.
		@param model The model to compute the bounding sphere for.
		@param matrix The matrix to use for its position.
	*/
	void ComputeData(class Model* model, const glm::mat4& matrix) override;

	/**
		Check intersection with another CollisionVolume.
		@param other The other CollisionVolume instance.
	*/
	bool Intersect(const CollisionVolume& other) const override;

	/**
		Check intersection with another BoundingSphere instance.
		@param other The other BoundingSphere instance to check intersection with.
	*/
	bool Intersect(const BoundingSphere& other) const override;

	/**
		Check if this collision volume intersects with another.
	*/
	bool Intersect(const class AxisAlignedBoundingBox& other) const override;

	/**
		Check if this collision volume intersects with another.
	*/
	bool Intersect(const class OrientedBoundingBox& other) const override;

	/**
		Initialize the bounding sphere with model data.
	*/
	void Initialize(class Model* model, const glm::mat4& matrix = glm::mat4(1.0f)) override;

	void SetColor(const glm::vec4& newColor);

	const glm::vec4& GetColor() const;

private:

	/**
		Cannot copy.	
	*/
	BoundingSphere(const BoundingSphere&) = delete;

	/**
		Cannot copy.
	*/
	BoundingSphere& operator=(const BoundingSphere&) = delete;

	/**
		Cannot move.
	*/
	BoundingSphere(BoundingSphere&&) = delete;

	/**
		Cannot move.
	*/
	BoundingSphere& operator=(BoundingSphere&&) = delete;

public:

private:

	/**
		All bounding spheres have a center...
	*/
	glm::vec3 center;
		
	glm::vec3 originalCenter;

	/**
		All bounding spheres have a radius...
	*/
	float radius;

	float scalar;

	ColoredStaticGraphicsObject* boundingSphereVolume;

	Model* ownerModel;

	Entity* owner;

};


















#endif