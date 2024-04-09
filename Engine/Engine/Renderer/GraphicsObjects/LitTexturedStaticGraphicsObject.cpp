#include "LitTexturedStaticGraphicsObject.h"

#include "../Memory/UniformBuffer.h"
#include "../Images/Texture.h"
#include "../Cameras/CameraManager.h"
#include "../Lights/DirectionalLight.h"
#include "../Lights/LightManager.h"

LitTexturedStaticGraphicsObject::LitTexturedStaticGraphicsObject(const Model* const model, Texture* const tex) :
	GraphicsObject(model),
	texture(tex),
	mvp(),
	light()
{
	type = ObjectTypes::GraphicsObjectType::LitTexturedStatic;
	mvp.model = glm::mat4(1.0f);
	shaderName = "LitTexturedStatic";
	InitializeDescriptorSets();
}

LitTexturedStaticGraphicsObject::~LitTexturedStaticGraphicsObject()
{
}

void LitTexturedStaticGraphicsObject::CreateTextures()
{
	textures.push_back(texture);
	texture->SetBinding(1U);
}

void LitTexturedStaticGraphicsObject::CreateUniformBuffers()
{
	UniformBuffer* mvpUniformBuffer = new UniformBuffer(sizeof(mvp), 0);
	mvpUniformBuffer->PersistentMap();
	uniformBuffers.push_back(mvpUniformBuffer);

	UniformBuffer* lightUniformBuffer = new UniformBuffer(sizeof(light), 2);
	lightUniformBuffer->PersistentMap();
	uniformBuffers.push_back(lightUniformBuffer);
}

void LitTexturedStaticGraphicsObject::Update()
{
	const Camera& cam = CameraManager::GetActiveCamera();

	mvp.model = translation * rotation * scale;

	mvp.view = cam.GetView();
	mvp.projection = cam.GetProjection();
	mvp.projection[1][1] *= -1;

	DirectionalLight* dirLight = LightManager::GetDirectionalLight("MainDirLight");

	light.color = glm::vec4(dirLight->GetColor(), 1.0f);
	light.direction = glm::vec4(dirLight->GetDirection(), 0.0f);
	light.ambient = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);

	uniformBuffers[0]->SetData(&mvp);
	uniformBuffers[1]->SetData(&light);
}
