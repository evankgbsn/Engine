#include "FeatureSearchTool.h"

#include "../UserInterfaceManager.h"
#include "../UserInterfaceItem.h"
#include "../../Renderer/Model/ModelManager.h"
#include "../../Renderer/Images/TextureManager.h"
#include "../../Renderer/Windows/WindowManager.h"
#include "../../Renderer/Windows/Window.h"
#include "../Editor/Editor.h"
#include "../Text.h"

FeatureSearchTool::FeatureSearchTool() :
	featureNamesSet(std::unordered_set<std::string>()),
	featureSearchBackground(nullptr),
	featureSearchBarText(nullptr)
{
	InitializeFeatureNamesSet();

	Texture* const backgroundTexture = TextureManager::LoadTexture("../Engine/Engine/Renderer/Images/FeatureSearchToolBackground.png", "FeatureSearchToolBackground");
	featureSearchBackground = UserInterfaceManager::CrateUserInterfaceItem("FeatureSearchToolBackground", ModelManager::GetModel("DefaultRectangleWithDepth"), backgroundTexture, { 0,0 });

	std::function<void()> whenFeatureSearchToolBackgroundReady = [this]()
	{
		SetPositionToCursorPosition();
	};

	featureSearchBackground->TransformReady(whenFeatureSearchToolBackgroundReady);
}

FeatureSearchTool::~FeatureSearchTool()
{
}

void FeatureSearchTool::Enable()
{
	featureSearchBackground->InquireVisibility(UserInterfaceItem::Visibility::Visible);
	SetPositionToCursorPosition();
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

void FeatureSearchTool::SetPositionToCursorPosition()
{
	Window* const mainWindow = WindowManager::GetWindow("MainWindow");

	if (mainWindow != nullptr)
	{
		glm::vec2 cursorPosition(mainWindow->GetCursorPosition());


		float windowOpenLocationX = glm::min(static_cast<float>(cursorPosition.x), static_cast<float>(mainWindow->GetWidth()));

		float windowHeight = static_cast<float>(mainWindow->GetHeight());
		cursorPosition.y = windowHeight - cursorPosition.y;

		float windowOpenLocationY = glm::min(static_cast<float>(cursorPosition.y), static_cast<float>(windowHeight));

		glm::vec2 imageTextureDimensions = featureSearchBackground->GetTextureDimensions();


		static bool calledOnce = true;

		if (calledOnce)
		{
			featureSearchBackground->Scale(imageTextureDimensions.x, imageTextureDimensions.y);
			featureSearchBackground->SetPosition(windowOpenLocationX, windowOpenLocationY);
			calledOnce = false;
		}
		else
		{
			featureSearchBackground->SetPosition(windowOpenLocationX, windowOpenLocationY);
		}

	}
}

void FeatureSearchTool::EnableInput()
{
}
