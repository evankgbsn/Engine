#ifndef TEXTUREDSTATICGRAPHICSOBJECT_H
#define TEXTUREDSTATICGRAPHICSOBJECT_H

#include "GraphicsObject.h"
#include "3DTransformable.h"

class Model;
class Texture;

class TexturedStaticGraphicsObject : public GraphicsObject, public Graphics3DTransformable
{
public:

	TexturedStaticGraphicsObject() = delete;

	TexturedStaticGraphicsObject(Model* const model, Texture* const texture);

	~TexturedStaticGraphicsObject();

	TexturedStaticGraphicsObject(const TexturedStaticGraphicsObject&) = delete;

	TexturedStaticGraphicsObject& operator=(const TexturedStaticGraphicsObject&) = delete;

	TexturedStaticGraphicsObject(TexturedStaticGraphicsObject&&) = delete;

	TexturedStaticGraphicsObject& operator=(TexturedStaticGraphicsObject&&) = delete;

	void Update() override;

	void Translate(const glm::vec3& translation) override;

	void Scale(const glm::vec3& scale) override;

protected:

	// Graphics3DTransformable

	void Rotate(float angle, const glm::vec3& axis) override;

	glm::vec3 GetTranslation() const override;

	glm::vec3 GetScale() const override;

	glm::mat4 GetRotation() const override;

	struct MVPUniformBuffer
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 projection;
	};

	void CreateTextures() override;

	void CreateUniformBuffers() override;

	MVPUniformBuffer mvp;

	Texture* texture;

private:

};

#endif // TEXTUREDSTATICGRAPHICSOBJECT_H