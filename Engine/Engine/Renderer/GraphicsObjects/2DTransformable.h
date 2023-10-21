#ifndef GRAPHICS2DTRANSFORMABLE_H
#define GRAPHICS2DTRANSFORMABLE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Graphics2DTransformable
{
public:

protected:

	virtual void Translate(const glm::vec2&) = 0;

	virtual void Scale(const glm::vec2&) = 0;

	virtual void Rotate(float angle) = 0;

	virtual glm::vec2 GetTranslation() const = 0;

	virtual glm::vec2 GetScale() const = 0;

	virtual float GetRotation() const = 0;

	float angle = 0;

private:

};

#endif