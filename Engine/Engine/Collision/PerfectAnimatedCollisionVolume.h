#ifndef PERFECTCOLLISIONVOLUME_H
#define PERFECTCOLLIISONVOLUME_H

#include "CollisionVolume.h"

#include <unordered_map>
#include <vector>
#include <string>

class BoundingSphere;
class AxisAlignedBoundingBox;
class OrientedBoundingBox;
class BoundingSphere;
class Model;
class ColoredAnimatedGraphicsObject;
class TexturedAnimatedGraphicsObject;

class PerfectAnimatedCollisionVolume : public CollisionVolume
{
public:

	PerfectAnimatedCollisionVolume(std::function<void(Entity* owner)> callback, Entity* owner, TexturedAnimatedGraphicsObject* go);

	~PerfectAnimatedCollisionVolume();

	bool Intersect(const CollisionVolume&) const override;

	bool Intersect(const BoundingSphere& other)const override;

	bool Intersect(const AxisAlignedBoundingBox& other)const override;

	bool Intersect(const OrientedBoundingBox& other)const override;

	void Initialize(const std::vector<Vertex, std::allocator<Vertex>>& vertices, const glm::mat4&, const Model* const model = nullptr) override;

	void ComputeData(const std::vector<Vertex>& verts, const glm::mat4& transform) override;

	void SetClip(unsigned int clipIndex);

	void SetAnimationSpeed(float newSpeed);

	void SetColor(const glm::vec4 newColor);

	const glm::vec4& GetColor() const;

private:

	PerfectAnimatedCollisionVolume(const PerfectAnimatedCollisionVolume&) = delete;

	PerfectAnimatedCollisionVolume& operator=(const PerfectAnimatedCollisionVolume&) = delete;

	PerfectAnimatedCollisionVolume(PerfectAnimatedCollisionVolume&&) = delete;

	PerfectAnimatedCollisionVolume& operator=(PerfectAnimatedCollisionVolume&&) = delete;

	const TexturedAnimatedGraphicsObject* const animatedGraphicsObject;

	ColoredAnimatedGraphicsObject* graphics;

	std::unordered_map<std::string, std::vector<std::vector<const Vertex*>>> trianglesOfJoints;

	std::unordered_map<std::string, BoundingSphere*> jointSpheres;
};


#endif // PERFECTCOLLISIONVOLUME_H