#ifndef GRAPHICS3DTRANSFORMABLE_H
#define GRAPHICS3DTRANSFORMABLE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Graphics3DTransformable
{
public:

protected:

	virtual void Translate(const glm::vec3&) = 0;

	virtual void Scale(const glm::vec3&) = 0;

	virtual void Rotate(const glm::vec3&) = 0;

	virtual const glm::vec3& GetTranslation() const = 0;

	virtual const glm::vec3& GetScale() const = 0;

	virtual const glm::mat4& GetRotation() const = 0;

private:

};

#endif