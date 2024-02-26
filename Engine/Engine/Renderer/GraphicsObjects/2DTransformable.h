#ifndef GRAPHICS2DTRANSFORMABLE_H
#define GRAPHICS2DTRANSFORMABLE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <functional>
#include <vector>
#include <chrono>

class Graphics2DTransformable
{
public:

protected:
	
	void TransformObject();

	virtual void OrderedTranslate(const std::function<void()>& translationFunction)
	{
		orderedTranslationQueue.push_back(std::make_pair(std::chrono::high_resolution_clock().now().time_since_epoch().count(), translationFunction));
	};
	
	virtual void OrderedRotate(const std::function<void()>& rotationFunction)
	{
		orderedRotationQueue.push_back(std::make_pair(std::chrono::high_resolution_clock::now().time_since_epoch().count(), rotationFunction));
	};

	virtual void OrderedScale(const std::function<void()>& scaleFunction)
	{
		orderedScaleQueue.push_back(std::make_pair(std::chrono::high_resolution_clock::now().time_since_epoch().count(), scaleFunction));
	};

	virtual void UnorderedTransform(const std::function<void()>& translationFunction)
	{
		unorderedTransformQueue.push_back(std::make_pair(std::chrono::high_resolution_clock().now().time_since_epoch().count(), translationFunction));
	};

	virtual glm::vec2 GetTranslation() const = 0;

	virtual glm::vec2 GetScale() const = 0;

	virtual float GetRotation() const = 0;

	virtual void SetTranslation(const glm::vec2& newtranslation) = 0;

	float angle = 0;

private:

	std::vector<std::pair<long long, std::function<void()>>> orderedTranslationQueue;

	std::vector<std::pair<long long, std::function<void()>>> orderedRotationQueue;

	std::vector<std::pair<long long, std::function<void()>>> orderedScaleQueue;

	std::vector<std::pair<long long, std::function<void()>>> unorderedTransformQueue;
};

#endif