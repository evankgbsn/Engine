#include "ColoredStaticGraphicsObject.h"

#include "../Memory/UniformBuffer.h"
#include "../Cameras/CameraManager.h"

#include "../Images/TextureManager.h"

void ColoredStaticGraphicsObject::Update()
{
	const Camera& cam = CameraManager::GetActiveCamera();

	mvp.model = translation * rotation * scale;

	mvp.view = cam.GetView();
	mvp.projection = cam.GetProjection();
	mvp.projection[1][1] *= -1;

	uniformBuffers[0]->SetData(&mvp);
	uniformBuffers[1]->SetData(&colorUB);
}

void ColoredStaticGraphicsObject::SetColor(const glm::vec4& newColor)
{
	colorUB.color = newColor;
}

const glm::vec4& ColoredStaticGraphicsObject::GetColor() const
{
	return colorUB.color;
}

void ColoredStaticGraphicsObject::CreateTextures()
{
}

void ColoredStaticGraphicsObject::CreateUniformBuffers()
{
	UniformBuffer* mvpUniformBuffer = new UniformBuffer(sizeof(mvp), 0);
	mvpUniformBuffer->PersistentMap();
	uniformBuffers.push_back(mvpUniformBuffer);

	UniformBuffer* colorUniformBuffer = new UniformBuffer(sizeof(colorUB), 1);
	colorUniformBuffer->PersistentMap();
	uniformBuffers.push_back(colorUniformBuffer);
}

ColoredStaticGraphicsObject::ColoredStaticGraphicsObject(const Model* const m, const glm::vec4& color) :
	GraphicsObject(m),
	mvp(),
	colorUB({color})
{
	type = ObjectTypes::GraphicsObjectType::ColoredStatic;
	mvp.model = glm::mat4(1.0f);
	shaderName = "ColoredStatic";
	InitializeDescriptorSets();
}

ColoredStaticGraphicsObject::~ColoredStaticGraphicsObject()
{
}
