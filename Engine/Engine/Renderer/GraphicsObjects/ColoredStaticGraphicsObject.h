#ifndef COLOREDSTATICGRAPHICSOBJECT_H
#define COLOREDSTATICGRAPHICSOBJECT_H

#include "GraphicsObject.h"
#include "3DTransformable.h"

class Model;

class ColoredStaticGraphicsObject : public GraphicsObject, public Graphics3DTransformable
{
public:

	ColoredStaticGraphicsObject() = delete;

	ColoredStaticGraphicsObject(const Model* const model, const glm::vec4& color);

	~ColoredStaticGraphicsObject();

	ColoredStaticGraphicsObject(const ColoredStaticGraphicsObject&) = delete;

	ColoredStaticGraphicsObject& operator=(const ColoredStaticGraphicsObject&) = delete;

	ColoredStaticGraphicsObject(ColoredStaticGraphicsObject&&) = delete;

	ColoredStaticGraphicsObject& operator=(ColoredStaticGraphicsObject&&) = delete;

	void Update() override;

	void SetColor(const glm::vec4& newColor);

	const glm::vec4& GetColor() const;

protected:

	void CreateTextures() final;

	void CreateUniformBuffers() override;
	
	struct MVPUniformBuffer
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 projection;
	};

	struct ColorUniformBuffer
	{
		glm::vec4 color;
	};

	MVPUniformBuffer mvp;

	ColorUniformBuffer colorUB;

private:

};

#endif // ColoredStaticGraphicsObject_H