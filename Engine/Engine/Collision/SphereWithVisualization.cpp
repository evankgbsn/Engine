#include "SphereWithVisualization.h"

#include "../Renderer/Model/ModelManager.h"
#include "../Renderer/Model/Model.h"
#include "../Renderer/GraphicsObjects/GraphicsObject.h"
#include "../Renderer/GraphicsObjects/ColoredStaticGraphicsObject.h"
#include "../Renderer/GraphicsObjects/GraphicsObjectManager.h"

SphereWithVisualization::SphereWithVisualization(GraphicsObject* go) :
	Sphere(go->GetModel()->GetVertices()),
	wrapedGraphics(go)
{
	GraphicsObjectManager::CreateColoredStaticGraphicsObject(ModelManager::GetModel("Sphere"), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), [this](ColoredStaticGraphicsObject* go)
		{
			go->SetScale({GetRadius(), GetRadius(), GetRadius()});
			go->SetTranslation(GetOrigin());
			GraphicsObjectManager::WireFrame(go, ObjectTypes::GraphicsObjectType::ColoredStatic);
			graphics = go;
		});
}

SphereWithVisualization::~SphereWithVisualization()
{
}

void SphereWithVisualization::Update(const glm::mat4& transformation)
{
	if (graphics->Loaded())
	{
		glm::mat4 scale(1.0f);
		scale = glm::scale(scale, { GetRadius(), GetRadius(), GetRadius() });

		glm::mat4 translation(1.0f);
		translation = glm::translate(translation, GetOffset());

		graphics->SetTransform(transformation * translation * scale);
		Transform(graphics->GetTransform());
	}
}
