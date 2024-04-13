#include "AnimatedCollider.h"

#include "../Renderer/GraphicsObjects/TexturedAnimatedGraphicsObject.h"

AnimatedCollider::AnimatedCollider(const TexturedAnimatedGraphicsObject* const graphicsObject) :
	wrapedGraphics(graphicsObject)
{
	InitializeOBBs();
}

AnimatedCollider::~AnimatedCollider()
{
}

void AnimatedCollider::InitializeOBBs()
{
}
