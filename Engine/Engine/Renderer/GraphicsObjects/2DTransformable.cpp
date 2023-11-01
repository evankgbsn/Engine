#include "2DTransformable.h"

void Graphics2DTransformable::TransformObject()
{
	for (const auto& translationFunction : translationQueue)
	{
		translationFunction();
	}

	translationQueue.clear();

	for (const auto& rotationFunction : rotationQueue)
	{
		rotationFunction();
	}

	rotationQueue.clear();

	for (const auto& scaleFunction : scaleQueue)
	{
		scaleFunction();
	}

	scaleQueue.clear();
}
