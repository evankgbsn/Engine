#ifndef CollisionVolume_h
#define CollisionVolume_h

#include <glm/glm.hpp>

#include <functional>

class BoundingSphere;
class AxisAlignedBoundingBox;
class OrientedBoundingBox;
class Entity;
class Vertex;
class ColoredStaticGraphicsObject;

/**
	The base class for all collision volumes. AABB, OBB, Bounding sphere.
*/
class CollisionVolume
{

public:

	/**
		The constructor.
	*/
	CollisionVolume(std::function<void(Entity* owner)> callback, Entity* owner);

	/**
		The destructor. Virtual for proper clean up of derived classes.
	*/
	virtual ~CollisionVolume();

	/**
		Get the center of the volume.
	*/
	virtual const glm::vec3& GetCenter() const = 0;

	/**
		Conpute the data for the CollisionVolume.
	*/
	virtual void ComputeData(const std::vector<Vertex>& vertices, const glm::mat4& matrix);

	/**
		Check if this collision volume intersects with another.
	*/
	virtual bool Intersect(const CollisionVolume& other) const = 0;

	/**
		Check if this collision volume intersects with another.
	*/
	virtual bool Intersect(const BoundingSphere& other) const = 0;

	/**
		Check if this collision volume intersects with another.
	*/
	virtual bool Intersect(const AxisAlignedBoundingBox& other) const = 0;

	/**
		Check if this colliison volume intersects with another.
	*/
	virtual bool Intersect(const OrientedBoundingBox& other) const = 0;

	/**
		Inisilaize the data for a collision volume with model data. Different collision volumes need to be initialized in different ways but all will have some sort of initialization.
	*/
	virtual void Initialize(const std::vector<Vertex>& vertices, const glm::mat4&) = 0;

	void SetColor(const glm::vec4& newColor);

	const glm::vec4& GetColor() const;

	void UpdateGraphicsTransform(const glm::mat4&);

	void Collide() const;

private:

	CollisionVolume() = delete;

	/**
		Cannot copy.
	*/
	CollisionVolume(const CollisionVolume&) = delete;

	/**
		Cannot copy.
	*/
	CollisionVolume& operator=(const CollisionVolume&) = delete;

	/**
		Cannot move.
	*/
	CollisionVolume(CollisionVolume&&) = delete;

	/**
		Cannot move.
	*/
	CollisionVolume& operator=(CollisionVolume&&) = delete;

protected:

	Entity* owningEntity;

	ColoredStaticGraphicsObject* graphics;

	std::function<void(Entity* owner)> collisionCallback;

public:

private:

protected:

};




























#endif