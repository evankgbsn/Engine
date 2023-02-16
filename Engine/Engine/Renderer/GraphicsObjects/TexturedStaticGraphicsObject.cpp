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
