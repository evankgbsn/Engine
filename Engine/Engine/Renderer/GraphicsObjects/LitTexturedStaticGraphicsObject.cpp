#include "LitTexturedStaticGraphicsObject.h"

#include "../Memory/UniformBuffer.h"
#include "../Images/Texture.h"
#include "../Cameras/CameraManager.h"
#include "../Lights/DirectionalLight.h"
#include "../Lights/LightManager.h"

LitTexturedStaticGraphicsObject::LitTexturedStaticGraphicsObject(Model* const model, Texture* const tex) :
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

void LitTexturedStaticGraphicsObject::Translate(const glm::vec3& translation)
{

}

void LitTexturedStaticGraphicsObject::Rotate(float angle, const glm::vec3& axis)
{

}

void LitTexturedStaticGraphicsObject::Scale(const glm::vec3& scale)
{

}

glm::vec3 LitTexturedStaticGraphicsObject::GetTranslation() const
{
	return glm::vec3();
}

glm::mat4 LitTexturedStaticGraphicsObject::GetRotation() const
{
	return glm::mat4();
}

glm::vec3 LitTexturedStaticGraphicsObject::GetScale() const
{
	return glm::vec3();
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
	const Camera& cam = CameraManager::GetCamera("MainCamera");

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
