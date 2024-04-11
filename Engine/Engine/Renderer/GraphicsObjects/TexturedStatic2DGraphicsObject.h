#ifndef TEXTUREDSTATIC2DGRAPHICSOBJECT_H
#define TEXTUREDSTATIC2DGRAPHICSOBJECT_H

#include "GraphicsObject.h"
#include "3DTransformable.h"

class Model;
class Texture;

class TexturedStatic2DGraphicsObject : public GraphicsObject, public Graphics3DTransformable
{
public:

	TexturedStatic2DGraphicsObject() = delete;

	TexturedStatic2DGraphicsObject(const Model* const model, Texture* const texture);

	~TexturedStatic2DGraphicsObject();

	TexturedStatic2DGraphicsObject(const TexturedStatic2DGraphicsObject&) = delete;

	TexturedStatic2DGraphicsObject& operator=(const TexturedStatic2DGraphicsObject&) = delete;

	TexturedStatic2DGraphicsObject(TexturedStatic2DGraphicsObject&&) = delete;

	TexturedStatic2DGraphicsObject& operator=(TexturedStatic2DGraphicsObject&&) = delete;

	void SetZOrder(float newZ = 0.0f);

	float GetZOrder() const;

	void Update() override;

	glm::mat4 GetModelMat4() const;

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