#include "TexturedStaticGraphicsObject.h"

#include "../Memory/UniformBuffer.h"
#include "../Cameras/CameraManager.h"
#include "../Images/Texture.h"

void TexturedStaticGraphicsObject::Update()
{
	const Camera& cam = CameraManager::GetActiveCamera();

	mvp.model = translation * rotation * scale;

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
	
	// The binding for the texture sampler is 1.
}

TexturedStaticGraphicsObject::TexturedStaticGraphicsObject(const Model* const m, Texture* const tex) :
	GraphicsObject(m),
	texture(tex),
	mvp()
{
	type = ObjectTypes::GraphicsObjectType::TexturedStatic;
	mvp.model = glm::mat4(1.0f);
	shaderName = "TexturedStatic";
	InitializeDescriptorSets();
}

TexturedStaticGraphicsObject::~TexturedStaticGraphicsObject()
{
}
