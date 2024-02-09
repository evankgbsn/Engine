#include "FeatureSearchTool.h"

#include "../UserInterfaceManager.h"
#include "../UserInterfaceItem.h"
#include "../../Renderer/Model/ModelManager.h"
#include "../../Renderer/Images/TextureManager.h"
#include "../../Renderer/Windows/WindowManager.h"
#include "../../Renderer/Windows/Window.h"
#include "../Text.h"

FeatureSearchTool::FeatureSearchTool() :
	featureNamesSet(std::unordered_set<std::string>()),
	featureSearchBackground(nullptr),
	featureSearchBarText(nullptr)
{
	InitializeFeatureNamesSet();
	
	Window* const mainWindow = WindowManager::GetWindow("MainWindow");

	glm::vec2 cursorPosition(0.0f);

	if (mainWindow != nullptr)
	{
		cursorPosition = mainWindow->GetCursorPosition();
	}

	featureSearchBackground = UserInterfaceManager::CrateUserInterfaceItem(std::string("FeatureSearchBackground"), ModelManager::GetModel("DefaultRectangelWithDepth"), TextureManager::GetTexture("Woman"), cursorPosition);
}

FeatureSearchTool::~FeatureSearchTool()
{
}

void FeatureSearchTool::Enable()
{

}

void FeatureSearchTool::InitializeFeatureNamesSet()
{
	static const std::unordered_set<std::string> featureNames = {};

	featureNamesSet = featureNames;
}

void FeatureSearchTool::LoadFeatureSearchToolModels() const
{
	//ModelManager::LoadModel();
}

void FeatureSearchTool::UpdateUserInterfaceItemsVisibility()
{
}
