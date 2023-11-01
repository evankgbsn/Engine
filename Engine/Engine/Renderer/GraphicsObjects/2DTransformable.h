#ifndef GRAPHICS2DTRANSFORMABLE_H
#define GRAPHICS2DTRANSFORMABLE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <functional>
#include <list>

class Graphics2DTransformable
{
public:

protected:
	
	void TransformObject();

	virtual void Translate(const std::function<void()>& translationFunction)
	{
		translationQueue.push_back(translationFunction);
	};
	
	virtual void Rotate(const std::function<void()>& rotationFunction)
	{
		rotationQueue.push_back(rotationFunction);
	};

	virtual void Scale(const std::function<void()>& scaleFunction)
	{
		scaleQueue.push_back(scaleFunction);
	};

	virtual glm::vec2 GetTranslation() const = 0;

	virtual glm::vec2 GetScale() const = 0;

	virtual float GetRotation() const = 0;

	float angle = 0;

private:

	std::list<std::function<void()>> translationQueue;

	std::list<std::function<void()>> rotationQueue;

	std::list<std::function<void()>> scaleQueue;

};

#endif