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

	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_A, characterInputFunction);
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_B, characterInputFunction);
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_C, characterInputFunction);
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_D, characterInputFunction);
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_E, characterInputFunction);
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_F, characterInputFunction);
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_G, characterInputFunction);
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_H, characterInputFunction);
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_I, characterInputFunction);
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_J, characterInputFunction);
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_K, characterInputFunction);
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_L, characterInputFunction);
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_M, characterInputFunction);
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_N, characterInputFunction);
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_O, characterInputFunction);
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_P, characterInputFunction);
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_Q, characterInputFunction);
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_R, characterInputFunction);
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_S, characterInputFunction);
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_T, characterInputFunction);
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_U, characterInputFunction);
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_V, characterInputFunction);
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_W, characterInputFunction);
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_X, characterInputFunction);
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_Y, characterInputFunction);
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_Z, characterInputFunction);
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_SPACE, characterInputFunction);

	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_LEFT_SHIFT, characterInputFunctionShiftHeld);
	InputManager::RegisterCallbackForKeyState(KEY_RELEASE, KEY_LEFT_SHIFT, characterInputFunctionShiftRelease);

	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_BACKSPACE, characterInputFunctionBackspace);
}

void FeatureSearchTool::DisableInput()
{
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_A, characterInputFunction);
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_B, characterInputFunction);
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_C, characterInputFunction);
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_D, characterInputFunction);
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_E, characterInputFunction);
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_F, characterInputFunction);
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_G, characterInputFunction);
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_H, characterInputFunction);
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_I, characterInputFunction);
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_J, characterInputFunction);
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_K, characterInputFunction);
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_L, characterInputFunction);
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_M, characterInputFunction);
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_N, characterInputFunction);
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_O, characterInputFunction);
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_P, characterInputFunction);
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_Q, characterInputFunction);
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_R, characterInputFunction);
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_S, characterInputFunction);
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_T, characterInputFunction);
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_U, characterInputFunction);
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_V, characterInputFunction);
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_W, characterInputFunction);
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_X, characterInputFunction);
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_Y, characterInputFunction);
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_Z, characterInputFunction);
	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_SPACE, characterInputFunction);

	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_LEFT_SHIFT, characterInputFunctionShiftHeld);
	InputManager::DeregisterCallbackForKeyState(KEY_RELEASE, KEY_LEFT_SHIFT, characterInputFunctionShiftRelease);

	InputManager::DeregisterCallbackForKeyState(KEY_PRESS, KEY_BACKSPACE, characterInputFunctionBackspace);

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
