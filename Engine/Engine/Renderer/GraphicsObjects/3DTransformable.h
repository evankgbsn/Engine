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

private:

};

#endif