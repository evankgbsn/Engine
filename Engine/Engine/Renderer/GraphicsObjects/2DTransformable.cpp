#include "2DTransformable.h"

void Graphics2DTransformable::TransformObject()
{
	for (const auto& translationFunction : orderedTranslationQueue)
	{
		translationFunction.second();
	}

	orderedTranslationQueue.clear();

	for (const auto& rotationFunction : orderedRotationQueue)
	{
		rotationFunction.second();
	}

	orderedRotationQueue.clear();

	for (const auto& scaleFunction : orderedScaleQueue)
	{
		scaleFunction.second();
	}

	orderedScaleQueue.clear();
	
	for (const auto& transformFunction : unorderedTransformQueue)
	{
		transformFunction.second();
	}

	unorderedTransformQueue.clear();

}
