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

	glm::vec2 backgroundOpenLocation = GetWindowOpenLocation();
	backgroundOpenLocation.x += 180.0f;

	featureSearchBackground = UserInterfaceManager::CrateUserInterfaceItem("FeatureSearchToolBackground", ModelManager::GetModel("DefaultRectangleWithDepth"), backgroundTexture, 9.0f, backgroundOpenLocation, { backgroundTexture->GetWidth(), backgroundTexture->GetHeight() });

	Window* const mainWindow = WindowManager::GetWindow("MainWindow");
	featureSearchBarText = new Text("", GetWindowOpenLocation(), 8.9f);

	EnableInput();
}

FeatureSearchTool::~FeatureSearchTool()
{
}

void FeatureSearchTool::Enable()
{
	featureSearchBackground->InquireVisibility(UserInterfaceItem::Visibility::Visible);
	featureSearchBarText->SetVisibility(UserInterfaceItem::Visibility::Visible);
	SetPositionToCursorPosition();
	featureSearchBarText->SetPosition(GetWindowOpenLocation());
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
	glm::vec2 backgroundOpenLocation = GetWindowOpenLocation();
	backgroundOpenLocation.x += 180.0f;
	featureSearchBackground->SetPosition(backgroundOpenLocation.x, backgroundOpenLocation.y);
}

void FeatureSearchTool::EnableInput()
{
	static bool shiftHeld = false;

	characterInputFunctionBackspace = new std::function<void(int)>([this](int keyCode)
	{
		featureSearchBarText->Backspace();
	});

	characterInputFunctionShiftHeld = new std::function<void(int)>([](int keyCode)
	{
		shiftHeld = true;
	});

	characterInputFunctionShiftRelease = new std::function<void(int)>([](int keyCode)
	{
		shiftHeld = false;
	});

	characterInputFunction = new std::function<void(int)>([this](int keyCode) 
	{
		int upperOrLower = ((shiftHeld || keyCode == 32) ? 0 : 32);
		featureSearchBarText->Append(std::string() + (char)(keyCode + upperOrLower), 8.9f);
	});

	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_A, characterInputFunction, "Type");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_B, characterInputFunction, "Type");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_C, characterInputFunction, "Type");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_D, characterInputFunction, "Type");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_E, characterInputFunction, "Type");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_F, characterInputFunction, "Type");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_G, characterInputFunction, "Type");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_H, characterInputFunction, "Type");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_I, characterInputFunction, "Type");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_J, characterInputFunction, "Type");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_K, characterInputFunction, "Type");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_L, characterInputFunction, "Type");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_M, characterInputFunction, "Type");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_N, characterInputFunction, "Type");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_O, characterInputFunction, "Type");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_P, characterInputFunction, "Type");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_Q, characterInputFunction, "Type");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_R, characterInputFunction, "Type");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_S, characterInputFunction, "Type");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_T, characterInputFunction, "Type");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_U, characterInputFunction, "Type");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_V, characterInputFunction, "Type");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_W, characterInputFunction, "Type");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_X, characterInputFunction, "Type");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_Y, characterInputFunction, "Type");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_Z, characterInputFunction, "Type");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_SPACE, characterInputFunction, "Type");

	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_LEFT_SHIFT, characterInputFunctionShiftHeld, "Type");
	InputManager::RegisterCallbackForKeyState(KEY_RELEASE, KEY_LEFT_SHIFT, characterInputFunctionShiftRelease, "Type");

	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_BACKSPACE, characterInputFunctionBackspace, "Type");
}

void FeatureSearchTool::DisableInput()
{
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_A, "Type");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_B, "Type");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_C, "Type");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_D, "Type");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_E, "Type");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_F, "Type");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_G, "Type");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_H, "Type");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_I, "Type");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_J, "Type");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_K, "Type");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_L, "Type");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_M, "Type");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_N, "Type");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_O, "Type");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_P, "Type");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_Q, "Type");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_R, "Type");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_S, "Type");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_T, "Type");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_U, "Type");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_V, "Type");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_W, "Type");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_X, "Type");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_Y, "Type");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_Z, "Type");
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_SPACE, "Type");
	
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_LEFT_SHIFT, "Type");
	InputManager::DeregisterCallbackForKeyState(KEY_RELEASE, KEY_LEFT_SHIFT, "Type");
	
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_BACKSPACE, "Type");
	
	delete characterInputFunction;
	delete characterInputFunctionShiftHeld;
	delete characterInputFunctionShiftRelease;
	delete characterInputFunctionBackspace;
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
