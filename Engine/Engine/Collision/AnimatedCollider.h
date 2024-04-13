#ifndef ANIMATEDCOLLIDER_H
#define ANIMATEDCOLLIDER_H

#include "Collider.h"

#include <unordered_map>
#include <string>

class TexturedAnimatedGraphicsObject;
class ColoredStaticGraphicsObject;
class OrientedBoundingBoxWithVisualization;

class AnimatedCollider : public Collider
{
public:

	AnimatedCollider(const TexturedAnimatedGraphicsObject* const graphicsObject);

	~AnimatedCollider();

private:

	void InitializeOBBs();

	AnimatedCollider() = delete;

	AnimatedCollider(const AnimatedCollider&) = delete;

	AnimatedCollider& operator=(const AnimatedCollider) = delete;

	AnimatedCollider(AnimatedCollider&&) = delete;

	AnimatedCollider& operator=(AnimatedCollider&&) = delete;

	const TexturedAnimatedGraphicsObject* wrapedGraphics;

	std::unordered_map<std::string, OrientedBoundingBoxWithVisualization*> obbs;

};

#endif // ANIMATEDCOLLIDER_H