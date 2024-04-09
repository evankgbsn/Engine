#ifndef LitTexturedStaticGraphicsObject_H
#define LitTexturedStaticGraphicsObject_H

#include "GraphicsObject.h"
#include "3DTransformable.h"

class LitTexturedStaticGraphicsObject : public GraphicsObject, public Graphics3DTransformable
{
public:

	LitTexturedStaticGraphicsObject(const Model* const model, Texture* const texture);

	~LitTexturedStaticGraphicsObject();

protected:

private:

	struct MVPUniformBuffer
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 projection;
	};

	struct LightUniformBuffer
	{
		glm::vec4 ambient;
		glm::vec4 direction;
		glm::vec4 color;
	};

	LitTexturedStaticGraphicsObject() = delete;

	LitTexturedStaticGraphicsObject(const LitTexturedStaticGraphicsObject&) = delete;

	LitTexturedStaticGraphicsObject& operator=(const LitTexturedStaticGraphicsObject&) = delete;

	LitTexturedStaticGraphicsObject(LitTexturedStaticGraphicsObject&&) = delete;

	LitTexturedStaticGraphicsObject& operator=(LitTexturedStaticGraphicsObject &&) = delete;

	void CreateTextures() override;

	void CreateUniformBuffers() override;

	void Update() override;

	MVPUniformBuffer mvp;

	LightUniformBuffer light;

	Texture* texture;
};


#endif //LitTexturedStaticGraphicsObject_H