#ifndef TEXTUREDSTATIC2DGRAPHICSOBJECT_H
#define TEXTUREDSTATIC2DGRAPHICSOBJECT_H

#include "GraphicsObject.h"
#include "2DTransformable.h"

class Model;
class Texture;

class TexturedStatic2DGraphicsObject : public GraphicsObject, private Graphics2DTransformable
{
public:

	TexturedStatic2DGraphicsObject() = delete;

	TexturedStatic2DGraphicsObject(Model* const model, Texture* const texture);

	~TexturedStatic2DGraphicsObject();

	TexturedStatic2DGraphicsObject(const TexturedStatic2DGraphicsObject&) = delete;

	TexturedStatic2DGraphicsObject& operator=(const TexturedStatic2DGraphicsObject&) = delete;

	TexturedStatic2DGraphicsObject(TexturedStatic2DGraphicsObject&&) = delete;

	TexturedStatic2DGraphicsObject& operator=(TexturedStatic2DGraphicsObject&&) = delete;

	void SetZOrder(float newZ = 0.0f);

	float GetZOrder() const;

	void TranslateObject(const glm::vec2& translation, float zOrder = 0.0f);

	void RotateObject(const float& rotation);

	void ScaleObject(const glm::vec2& scale);

	void TranslateObjectUnordered(const glm::vec2& translation, float zOrder = 0.0f);

	void RotateObjectUnordered(const float& rotation);

	void ScaleObjectUnordered(const glm::vec2& scale);

	void Update() override;

	glm::vec2 GetTranslation() const override;

	glm::vec2 GetScale() const override;

	float GetRotation() const override;

	glm::mat4 GetModelMat4() const;

	void SetTranslation(const glm::vec2& newTranslation) override;

	const Texture* const GetTexture() const;

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