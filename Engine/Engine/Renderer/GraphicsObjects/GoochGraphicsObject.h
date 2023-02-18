#ifndef GOOCHGRAPHICSOBJECT_H
#define GOOCHGRAPHICSOBJECT_H

#include "GraphicsObject.h"

class GoochGraphicsObject : public GraphicsObject
{

public:

	GoochGraphicsObject() = delete;

	GoochGraphicsObject(Model* const model, Texture* const texture);

	~GoochGraphicsObject();

	GoochGraphicsObject(const GoochGraphicsObject&) = delete;

	GoochGraphicsObject& operator=(const GoochGraphicsObject&) = delete;

	GoochGraphicsObject(GoochGraphicsObject&&) = delete;

	GoochGraphicsObject& operator=(GoochGraphicsObject&&) = delete;

private:

	struct MVPUniformBuffer
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 projection;
	};

	virtual void CreateUniformBuffers() override;

	virtual void CreateTextures() override;

	virtual void Update() override;

	MVPUniformBuffer mvp;

	Texture* texture;
};

#endif // GOOCHCRAPHICSOBJECT_H