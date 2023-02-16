#ifndef TEXTUREDSTATICGRAPHICSOBJECT_H
#define TEXTUREDSTATICGRAPHICSOBJECT_H

#include "GraphicsObject.h"

class Model;
class Texture;

class TexturedStaticGraphicsObject : public GraphicsObject
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

protected:

	struct MVPUniformBuffer
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 projection;
	};

	virtual void CreateTextures() override;

	virtual void CreateUniformBuffers() override;

	MVPUniformBuffer mvp;

	Texture* texture;

private:

};


#endif // TEXTUREDSTATICGRAPHICSOBJECT_H