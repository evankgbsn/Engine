#ifndef TEXTUREDSTATIC2DGRAPHICSOBJECT_H
#define TEXTUREDSTATIC2DGRAPHICSOBJECT_H

#include "GraphicsObject.h"
#include "2DTransformable.h"

class Model;
class Texture;

class TexturedStatic2DGraphicsObject : public GraphicsObject, public Graphics2DTransformable
{
public:

	TexturedStatic2DGraphicsObject() = delete;

	TexturedStatic2DGraphicsObject(Model* const model, Texture* const texture);

	~TexturedStatic2DGraphicsObject();

	TexturedStatic2DGraphicsObject(const TexturedStatic2DGraphicsObject&) = delete;

	TexturedStatic2DGraphicsObject& operator=(const TexturedStatic2DGraphicsObject&) = delete;

	TexturedStatic2DGraphicsObject(TexturedStatic2DGraphicsObject&&) = delete;

	TexturedStatic2DGraphicsObject& operator=(TexturedStatic2DGraphicsObject&&) = delete;

	void Update() override;

	// Graphics2DTransformable
	void Translate(const glm::vec2& translation) override;

	void Scale(const glm::vec2& scale) override;

	void Rotate(float angle) override;

	glm::vec2 GetTranslation() const override;

	glm::vec2 GetScale() const override;

	float GetRotation() const override;

private:
	
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
};

#endif // TEXTUREDSTATIC2DGRAPHICSOBJECT_H