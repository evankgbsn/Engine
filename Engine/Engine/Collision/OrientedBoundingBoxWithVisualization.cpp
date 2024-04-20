#include "OrientedBoundingBoxWithVisualization.h"

#include "../Renderer/Model/ModelManager.h"
#include "../Renderer/GraphicsObjects/ColoredStaticGraphicsObject.h"
#include "../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "../Renderer/Model/Vertex.h"


OrientedBoundingBoxWithVisualization::OrientedBoundingBoxWithVisualization(const glm::vec3& initialOrigin, const glm::vec3& initialSize, const glm::mat4& initialOrientation) :
	OrientedBoundingBox(initialOrigin, initialSize, initialOrientation)
{
	CreateGraphics();
}

OrientedBoundingBoxWithVisualization::OrientedBoundingBoxWithVisualization(const std::vector<Vertex>& vertices, const glm::mat4& initialOrientation) :
	OrientedBoundingBox(vertices, initialOrientation)
{
	CreateGraphics();
}

OrientedBoundingBoxWithVisualization::~OrientedBoundingBoxWithVisualization()
{
	GraphicsObjectManager::DeleteGraphicsObject(graphics);
}

void OrientedBoundingBoxWithVisualization::Update(const glm::mat4& transform)
{
	if (graphics->Loaded())
	{
		glm::mat4 scale(1.0f);
		scale = glm::scale(scale, GetSize());

		glm::mat4 translation(1.0f);
		translation = glm::translate(translation, GetOffset());

		graphics->SetTransform(transform * translation * scale);

		SetOrigin(graphics->GetTranslation());
		SetOrientation(graphics->GetRotation());
	}
}

void OrientedBoundingBoxWithVisualization::ToggleVisibility()
{
	GraphicsObjectManager::ToggleGraphicsObjectDraw(graphics, ObjectTypes::GraphicsObjectType::ColoredStatic);
}

void OrientedBoundingBoxWithVisualization::SetColor(const glm::vec4& newColor)
{
	if (graphics != nullptr)
	{
		graphics->SetColor(newColor);
	}
}

void OrientedBoundingBoxWithVisualization::CreateGraphics()
{
	GraphicsObjectManager::CreateColoredStaticGraphicsObject(ModelManager::GetModel("Cube"), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), [this](ColoredStaticGraphicsObject* go)
		{
			GraphicsObjectManager::WireFrame(go, ObjectTypes::GraphicsObjectType::ColoredStatic);
			graphics = go;
			graphics->SetScale(GetSize());
			graphics->SetTranslation(GetOffset());
			SetOrigin(GetOffset());
		});
}
