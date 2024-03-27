#include "Editor.h"

#include "../../Utils/Logger.h"
#include "../../UI/UserInterfaceManager.h"
#include "../../UI/UserInterfaceItem.h"
#include "../../Renderer/Images/TextureManager.h"
#include "../../Renderer/Model/ModelManager.h"
#include "../../Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "../../Renderer/GraphicsObjects/TexturedStatic2DGraphicsObject.h"
#include "FeatureSearchTool.h"

Editor* Editor::instance = nullptr;

void Editor::Initialize()
{
	if (instance == nullptr)
	{
		instance = new Editor();
		Logger::Log(std::string("Initialized Editor"), Logger::Category::Success);
	}
	else
	{
		Logger::Log(std::string("Calling Editor::Initialze() before Editor::Terminate()"), Logger::Category::Warning);
	}
}

void Editor::Terminate()
{
	if (instance != nullptr)
	{
		delete instance;
		Logger::Log(std::string("Terminated Editor"), Logger::Category::Success);
	}
	else
	{
		Logger::Log(std::string("Calling Editor::Terminate() before Logger::Initialize()"), Logger::Category::Warning);
	}
}

bool Editor::Operating()
{
	return instance != nullptr ? instance->operating : false;
}

void Editor::Open()
{
	if (Operating())
	{
		instance->featureSearchTool->Enable();
		instance->open = true;
		Logger::Log(std::string("Opened Editor"), Logger::Category::Info);
	}
}

void Editor::Close()
{
	if (Operating())
	{
		instance->featureSearchTool->Disable();
		instance->open = false;
		Logger::Log(std::string("Closed Editor"), Logger::Category::Info);
	}
}

bool Editor::IsOpen()
{
	return Operating() ? instance->open : false;
}

Editor::Editor() :
	featureSearchTool(new FeatureSearchTool()),
	operating(true),
	open(true)
{
	UserInterfaceItem* item = new UserInterfaceItem("TestTextBox", ModelManager::GetModel("DefaultRectangleWithDepth"), TextureManager::GetTexture("Woman"), { 300.0f, 300.0f });

	//GraphicsObjectManager::CreateTexturedStatic2DGraphicsObject(ModelManager::GetModel("DefaultRectangleWithDepth"), TextureManager::GetTexture("Woman"), [](GraphicsObject* go)
	//{
	//	TexturedStatic2DGraphicsObject* tsgo = static_cast<TexturedStatic2DGraphicsObject*>(go);
	//	tsgo->ScaleObject({ 300.0f, 300.0f });
	//	tsgo->TranslateObject({ 500.0f, 500.0f });
	//});
}

Editor::~Editor()
{
	delete featureSearchTool;
	operating = false;
}
