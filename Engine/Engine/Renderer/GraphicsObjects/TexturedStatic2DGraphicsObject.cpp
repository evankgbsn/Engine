#include "TexturedStatic2DGraphicsObject.h"

#include "../Cameras/CameraManager.h"
#include "../Memory/UniformBuffer.h"
#include "../Images/Texture.h"

#include <glm/gtc/matrix_transform.hpp>

TexturedStatic2DGraphicsObject::TexturedStatic2DGraphicsObject(const Model* const m, Texture* const tex) :
	GraphicsObject(m),
	texture(tex),
	mvp()
{
	type = ObjectTypes::GraphicsObjectType::TexturedStatic2D;
	mvp.model = glm::mat4(1.0f);
	shaderName = "TexturedStatic"; //TODO: Maybe seperate glsl/spv shader for 2D texture static graphics objects.
	InitializeDescriptorSets();
}

TexturedStatic2DGraphicsObject::~TexturedStatic2DGraphicsObject()
{
}

void TexturedStatic2DGraphicsObject::SetZOrder(float newZ)
{
	Translate(glm::vec3(0.0f, 0.0f, -mvp.model[3].z));
	Translate(glm::vec3(0.0f, 0.0f, newZ));
}

float TexturedStatic2DGraphicsObject::GetZOrder() const
{
	return mvp.model[3].z;
}



void TexturedStatic2DGraphicsObject::Update()
{
	const Camera& cam = CameraManager::GetCamera("MainOrthoCamera");

	mvp.model = translation * rotation * scale;

	mvp.view = cam.GetView();
	mvp.projection = cam.GetProjection();
	mvp.projection[1][1] *= -1;

	uniformBuffers[0]->SetData(&mvp);
}

void TexturedStatic2DGraphicsObject::CreateTextures()
{
	textures.push_back(texture);
	texture->SetBinding(1U);
}

void TexturedStatic2DGraphicsObject::CreateUniformBuffers()
{
	UniformBuffer* mvpUniformBuffer = new UniformBuffer(sizeof(mvp), 0);
	mvpUniformBuffer->PersistentMap();
	uniformBuffers.push_back(mvpUniformBuffer);

	// The binding for the texture sampler is 1.
}

glm::mat4 TexturedStatic2DGraphicsObject::GetModelMat4() const
{
	return mvp.model;
}

const Texture* const TexturedStatic2DGraphicsObject::GetTexture() const
{
	return texture;
}
