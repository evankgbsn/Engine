#include "TexturedStaticGraphicsObject.h"

#include "../Memory/UniformBuffer.h"
#include "../Cameras/CameraManager.h"
#include "../Images/Texture.h"

void TexturedStaticGraphicsObject::Update()
{
	const Camera& cam = CameraManager::GetCamera("MainCamera");

	mvp.view = cam.GetView();
	mvp.projection = cam.GetProjection();
	mvp.projection[1][1] *= -1;

	uniformBuffers[0]->SetData(&mvp);
}

void TexturedStaticGraphicsObject::Translate(const glm::vec3& translation)
{
	mvp.model = glm::translate(mvp.model, translation);
}

void TexturedStaticGraphicsObject::Scale(const glm::vec3& scale)
{
	mvp.model = glm::scale(mvp.model, scale);
}

void TexturedStaticGraphicsObject::Rotate(float angle, const glm::vec3& axis)
{
	
}

glm::vec3 vec3DefaultReturn;
glm::mat4 mat4DefaultReturn;

glm::vec3 TexturedStaticGraphicsObject::GetTranslation() const
{
	return vec3DefaultReturn;
}

glm::vec3 TexturedStaticGraphicsObject::GetScale() const
{
	return vec3DefaultReturn;
}

glm::mat4 TexturedStaticGraphicsObject::GetRotation() const
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
	UniformBuffer* mvpUniformBuffer = new UniformBuffer(sizeof(mvp), 0);
	mvpUniformBuffer->PersistentMap();
	uniformBuffers.push_back(mvpUniformBuffer);
	
	// The binding for the texture sampler is 1.
}

TexturedStaticGraphicsObject::TexturedStaticGraphicsObject(Model* const m, Texture* const tex) :
	GraphicsObject(m),
	texture(tex),
	mvp()
{
	mvp.model = glm::mat4(1.0f);
	shaderName = "TexturedStatic";
	InitializeDescriptorSets();
}

TexturedStaticGraphicsObject::~TexturedStaticGraphicsObject()
{
}
