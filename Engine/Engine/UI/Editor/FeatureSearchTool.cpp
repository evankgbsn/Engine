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

		featureSearchBackground = UserInterfaceManager::CrateUserInterfaceItem("FeatureSearchToolBackground", ModelManager::GetModel("DefaultRectangleWithDepth"), TextureManager::GetTexture("Woman"), { 300.0f, 300.0f });

		std::function<void()> whenReady = [this]()
		{
			//featureSearchBackground->Scale(100.0f, 100.0f);
		};

		featureSearchBackground->TransformReady(whenReady);
	}
}

FeatureSearchTool::~FeatureSearchTool()
{
}

void FeatureSearchTool::Enable()
{
	featureSearchBackground->InquireVisibility(UserInterfaceItem::Visibility::Visible);
}

void FeatureSearchTool::Disable()
{
	featureSearchBackground->InquireVisibility(UserInterfaceItem::Visibility::Invisible);
}

void FeatureSearchTool::InitializeFeatureNamesSet()
{
	static const std::unordered_set<std::string> featureNames = {};

	featureNamesSet = featureNames;
}

void FeatureSearchTool::LoadFeatureSearchToolModels() const
{
}

void FeatureSearchTool::UpdateUserInterfaceItemsVisibility()
{
}
