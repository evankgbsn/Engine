#ifndef GOOCHGRAPHICSOBJECT_H
#define GOOCHGRAPHICSOBJECT_H

#include "GraphicsObject.h"
#include "3DTransformable.h"

class GoochGraphicsObject : public GraphicsObject, public Graphics3DTransformable
{

public:

	GoochGraphicsObject() = delete;

	GoochGraphicsObject(Model* const model, Texture* const texture);

	~GoochGraphicsObject();

	GoochGraphicsObject(const GoochGraphicsObject&) = delete;

	GoochGraphicsObject& operator=(const GoochGraphicsObject&) = delete;

	GoochGraphicsObject(GoochGraphicsObject&&) = delete;

	GoochGraphicsObject& operator=(GoochGraphicsObject&&) = delete;

	void Translate(const glm::vec3&) override;

	void Scale(const glm::vec3&) override;

	void Rotate(const glm::vec3&) override;

	const glm::vec3& GetTranslation() const override;

	const glm::vec3& GetScale() const override;

	const glm::mat4& GetRotation() const override;

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