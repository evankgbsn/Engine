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

	virtual void Update() override;

	void Translate(const glm::vec3&) override;

protected:

	// Graphics3DTransformable
	

	void Scale(const glm::vec3&) override;

	void Rotate(const glm::vec3&) override;

	const glm::vec3& GetTranslation() const override;

	const glm::vec3& GetScale() const override;

	const glm::mat4& GetRotation() const override;

	struct MVPUniformBuffer
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 projection;
	};

	struct LightUniformBuffer
	{
		glm::vec3 direction;
		glm::vec3 color;
	};

	virtual void CreateTextures() override;

	virtual void CreateUniformBuffers() override;

	MVPUniformBuffer* mvp;

	LightUniformBuffer* light;

	Texture* texture;

private:

};


#endif // TEXTUREDSTATICGRAPHICSOBJECT_H