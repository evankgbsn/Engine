#include "FeatureSearchTool.h"

#include "../UserInterfaceManager.h"
#include "../UserInterfaceItem.h"
#include "../../Renderer/Model/ModelManager.h"
#include "../../Renderer/Images/TextureManager.h"
#include "../../Renderer/Images/Texture.h"
#include "../../Renderer/Windows/WindowManager.h"
#include "../../Input/InputManager.h"
#include "../../Renderer/Windows/Window.h"
#include "../Editor/Editor.h"
#include "../Text.h"

FeatureSearchTool::FeatureSearchTool() :
	featureNamesSet(std::unordered_set<std::string>()),
	featureSearchBackground(nullptr),
	featureSearchBarText(nullptr),
	characterInputFunction(nullptr)
{
	InitializeFeatureNamesSet();

	Texture* const backgroundTexture = TextureManager::GetTexture("FeatureSearchToolBackground");

	featureSearchBackground = UserInterfaceManager::CrateUserInterfaceItem("FeatureSearchToolBackground", ModelManager::GetModel("DefaultRectangleWithDepth"), backgroundTexture, GetWindowOpenLocation(), { backgroundTexture->GetWidth(), backgroundTexture->GetHeight() });

	Window* const mainWindow = WindowManager::GetWindow("MainWindow");
	featureSearchBarText = new Text("",mainWindow->GetCursorPosition());
}

FeatureSearchTool::~FeatureSearchTool()
{
}

void FeatureSearchTool::Enable()
{
	featureSearchBackground->InquireVisibility(UserInterfaceItem::Visibility::Visible);
	featureSearchBarText->SetVisibility(UserInterfaceItem::Visibility::Visible);
	SetPositionToCursorPosition();
	EnableInput();
}

void FeatureSearchTool::Disable()
{
	DisableInput();
	featureSearchBackground->InquireVisibility(UserInterfaceItem::Visibility::Invisible);
	featureSearchBarText->SetVisibility(UserInterfaceItem::Visibility::Invisible);
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
	glm::vec2 windowOpenLocation = GetWindowOpenLocation();
	featureSearchBackground->SetPosition(windowOpenLocation.x, windowOpenLocation.y);
}

void FeatureSearchTool::EnableInput()
{
	characterInputFunction = new std::function<void(int)>([this](int keyCode) 
	{
		featureSearchBarText->Append(std::string() + (char)keyCode);
		featureSearchBarText->SetSize(25.0f);
	});

	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_O, characterInputFunction);

}

void FeatureSearchTool::DisableInput()
{
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_O, characterInputFunction);

	delete characterInputFunction;
}

glm::vec2 FeatureSearchTool::GetWindowOpenLocation() const
{
	float windowOpenLocationY = 0.0f;
	float windowOpenLocationX = 0.0f;

	Window* const mainWindow = WindowManager::GetWindow("MainWindow");
	
	if (mainWindow != nullptr)
	{
		glm::vec2 cursorPosition(mainWindow->GetCursorPosition());

		windowOpenLocationX = glm::min(static_cast<float>(cursorPosition.x), static_cast<float>(mainWindow->GetWidth()));

		float windowHeight = static_cast<float>(mainWindow->GetHeight());
		cursorPosition.y = windowHeight - cursorPosition.y;

		windowOpenLocationY = glm::min(static_cast<float>(cursorPosition.y), static_cast<float>(windowHeight));
	}

	return glm::vec2(windowOpenLocationX, windowOpenLocationY);
}
