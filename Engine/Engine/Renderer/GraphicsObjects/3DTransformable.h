#ifndef GRAPHICS3DTRANSFORMABLE_H
#define GRAPHICS3DTRANSFORMABLE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Graphics3DTransformable
{
public:

	virtual void Translate(const glm::vec3& t)
	{
		translation = glm::translate(translation, t);
	};

	virtual void Rotate(float angle, const glm::vec3& axis)
	{
		float angleInRadians = angle / 57.2957795f;
		rotation = glm::rotate(rotation, angleInRadians, axis);
	};

	virtual void Scale(const glm::vec3& s)
	{
		scale = glm::scale(scale, s);
	};

	virtual void SetTranslation(const glm::vec3& t)
	{
		translation = glm::mat4(1.0f);
		translation = glm::translate(translation, t);
	};

	virtual void SetRotation(const glm::mat4& newRotation)
	{
		rotation = newRotation;
	};

	virtual void SetScale(const glm::vec3& s)
	{
		scale = glm::mat4(1.0f);
		scale = glm::scale(scale, s);
	};

	virtual glm::vec3 GetTranslation() const
	{
		return translation[3];
	};

	virtual glm::vec3 GetScale() const
	{
		return glm::vec3(scale[0][0], scale[1][1], scale[2][2]);
	};

	virtual glm::mat4 GetRotation() const
	{
		return rotation;
	};

	virtual void SetTransform(const glm::mat4& newTransform)
	{
		translation = glm::translate(glm::mat4(1.0f), glm::vec3(newTransform[3]));
		
		float scaleX = glm::length(newTransform[0]);
		float scaleY = glm::length(newTransform[1]);
		float scaleZ = glm::length(newTransform[2]);

		scale[0] = glm::vec4(scaleX, 0.0f, 0.0f, 0.0f);
		scale[1] = glm::vec4(0.0f, scaleY, 0.0f, 0.0f);
		scale[2] = glm::vec4(0.0f, 0.0f, scaleZ, 0.0f);

		rotation = glm::mat4(1.0f);

		rotation[0] = newTransform[0] / scaleX;
		rotation[1] = newTransform[1] / scaleY;
		rotation[2] = newTransform[2] / scaleZ;
	};

	virtual const glm::mat4& GetTransform()
	{
		return transformation = translation * scale * rotation;
	};

protected:

	Graphics3DTransformable() :
		translation(1.0f),
		rotation(1.0f),
		scale(1.0f)
	{

	}

	glm::mat4 translation;

	glm::mat4 rotation;

	glm::mat4 scale;

	glm::mat4 transformation;

private:

};

#endif