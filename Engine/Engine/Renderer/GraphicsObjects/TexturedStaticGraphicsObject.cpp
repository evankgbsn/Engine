#include "TexturedStaticGraphicsObject.h"

#include "../Memory/UniformBuffer.h"
#include "../Cameras/CameraManager.h"
#include "../Images/Texture.h"

void TexturedStaticGraphicsObject::Update()
{
	mvp.model = glm::mat4(1.0f);

	const Camera& cam = CameraManager::GetCamera("MainCamera");

	mvp.view = cam.GetView();
	mvp.projection = cam.GetProjection();
	mvp.projection[1][1] *= -1;

	uniformBuffers[0]->SetData(&mvp);
}

void TexturedStaticGraphicsObject::Translate(const glm::vec3&)
{
}

void TexturedStaticGraphicsObject::Scale(const glm::vec3&)
{
}

void TexturedStaticGraphicsObject::Rotate(const glm::vec3&)
{
}

glm::vec3 vec3DefaultReturn;
glm::mat4 mat4DefaultReturn;

const glm::vec3& TexturedStaticGraphicsObject::GetTranslation() const
{
	return vec3DefaultReturn;
}

const glm::vec3& TexturedStaticGraphicsObject::GetScale() const
{
	return vec3DefaultReturn;
}

const glm::mat4& TexturedStaticGraphicsObject::GetRotation() const
{
	return mat4DefaultReturn;
}

void TexturedStaticGraphicsObject::CreateTextures()
{
	textures.push_back(texture);
	texture->SetBinding(1U);
}

void TexturedStaticGraphicsObject::CreateUniformBuffers()
{
	// The binding for the texture sampler is 1.

	UniformBuffer* mvpUniformBuffer = new UniformBuffer(sizeof(mvp), 0);
	mvpUniformBuffer->PersistentMap();
	uniformBuffers.push_back(mvpUniformBuffer);

	UniformBuffer* lightUniformBuffer = new UniformBuffer(sizeof(light), 2);
	lightUniformBuffer->PersistentMap();
	uniformBuffers.push_back(lightUniformBuffer);
}

TexturedStaticGraphicsObject::TexturedStaticGraphicsObject(Model* const m, Texture* const tex) :
	GraphicsObject(m),
	texture(tex),
	mvp()
{
	shaderName = "TexturedStatic";
	InitializeDescriptorSets();
}

TexturedStaticGraphicsObject::~TexturedStaticGraphicsObject()
{
}
