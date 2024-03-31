#include "GoochGraphicsObject.h"

#include "../Cameras/CameraManager.h"
#include "../Memory/UniformBuffer.h"
#include "../Images/Texture.h"

#include <glm/gtc/matrix_transform.hpp>

void GoochGraphicsObject::CreateUniformBuffers()
{
	UniformBuffer* mvpUniformBuffer = new UniformBuffer(sizeof(mvp), 0);
	mvpUniformBuffer->PersistentMap();
	uniformBuffers.push_back(mvpUniformBuffer);
}

void GoochGraphicsObject::CreateTextures()
{
	textures.push_back(texture);
	texture->SetBinding(1U);
}

void GoochGraphicsObject::Update()
{
	const Camera& cam = CameraManager::GetActiveCamera();

	mvp.view = cam.GetView();
	mvp.projection = cam.GetProjection();
	mvp.projection[1][1] *= -1;

	uniformBuffers[0]->SetData(&mvp);
}

GoochGraphicsObject::GoochGraphicsObject(Model* const model, Texture* const tex) :
	GraphicsObject(model),
	mvp(),
	texture(tex)
{
	type = ObjectTypes::GraphicsObjectType::Gooch;
	shaderName = "Gooch";
	InitializeDescriptorSets();
	mvp.model = glm::mat4(1.0f);
}

GoochGraphicsObject::~GoochGraphicsObject()
{
}
