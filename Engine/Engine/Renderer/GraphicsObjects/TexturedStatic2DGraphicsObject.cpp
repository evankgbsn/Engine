#include "TexturedStatic2DGraphicsObject.h"

#include "../Cameras/CameraManager.h"
#include "../Memory/UniformBuffer.h"
#include "../Images/Texture.h"

#include <glm/gtc/matrix_transform.hpp>

TexturedStatic2DGraphicsObject::TexturedStatic2DGraphicsObject(Model* const m, Texture* const tex) :
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

void TexturedStatic2DGraphicsObject::TranslateObject(const glm::vec2& translation)
{
	std::function<void()> translationFunction = std::function<void()>([translation, this]()
	{
		mvp.model = glm::translate(mvp.model, glm::vec3(translation.x, translation.y, 0.0f));
	});

	Translate(translationFunction);
}

void TexturedStatic2DGraphicsObject::RotateObject(const float& rotation)
{
	std::function<void()> rotationFunction = std::function<void()>([rotation, this]()
	{
		static const glm::vec3 forward(0.0f, 0.0f, 1.0f);
		mvp.model = glm::rotate(mvp.model, rotation, forward);
		angle += rotation;
	});

	Rotate(rotationFunction);
}

void TexturedStatic2DGraphicsObject::ScaleObject(const glm::vec2& scale)
{
	std::function<void()> scaleFunction = std::function<void()>([scale, this]()
	{
		mvp.model = glm::scale(mvp.model, glm::vec3(scale, 1.0f));
	});

	Scale(scaleFunction);
}

void TexturedStatic2DGraphicsObject::Update()
{
	TransformObject();

	const Camera& cam = CameraManager::GetCamera("MainOrthoCamera");

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

glm::vec2 TexturedStatic2DGraphicsObject::GetTranslation() const
{
	return glm::vec2(mvp.model[3].x, mvp.model[3].y);
}

glm::vec2 TexturedStatic2DGraphicsObject::GetScale() const
{
	return glm::vec2(glm::length(mvp.model[0]), glm::length(mvp.model[1]));
}

float TexturedStatic2DGraphicsObject::GetRotation() const
{
	return angle;
}

glm::mat4 TexturedStatic2DGraphicsObject::GetModelMat4() const
{
	return mvp.model;
}
