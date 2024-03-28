#include "Player.h"

#include "Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "Renderer/GraphicsObjects/TexturedAnimatedGraphicsObject.h"
#include "Renderer/Model/ModelManager.h"
#include "Renderer/Cameras/CameraManager.h"
#include "Renderer/Cameras/Camera.h"
#include "Renderer/Images/TextureManager.h"
#include "Input/InputManager.h"

Player::Player() :
	wPress(nullptr),
	aPress(nullptr),
	sPress(nullptr),
	dPress(nullptr),
	moveSpeed(0.005f)
{
	auto onGraphicsObjectCreate = [this](GraphicsObject* go)
	{
		player = static_cast<TexturedAnimatedGraphicsObject*>(go);
		player->SetClip(0);
		player->SetAnimationSpeed(1.0f);
	};

	GraphicsObjectManager::CreateTexturedAnimatedGraphicsObject(ModelManager::GetModel("Woman"), TextureManager::GetTexture("Woman2"), onGraphicsObjectCreate);

	auto setCamera = [this]()
	{
		Camera& cam = CameraManager::GetCamera("MainCamera");

		if (player != nullptr)
		{
			glm::vec3 translation = player->GetTranslation();
			cam.SetTarget(translation);
			//cam.SetPosition(translation + glm::vec3(0.0f, 10.0f, -15.0f));
		}
	};

	wPress = new std::function<void(int)>([this, setCamera](int keyCode)
	{
		player->Translate(glm::vec3(0.0f, 0.0f, moveSpeed));
		player->SetRotation(glm::mat4(1.0f));
		setCamera();
	});

	aPress = new std::function<void(int)>([this, setCamera](int keyCode)
	{
		player->Translate(glm::vec3(moveSpeed, 0.0f, 0.0f));
		setCamera();
	});
	
	sPress = new std::function<void(int)>([this, setCamera](int keyCode)
	{
		player->Translate(glm::vec3(0.0f, 0.0f, -moveSpeed));
		player->SetRotation(glm::rotate(player->GetRotation(), 3.1415f, glm::vec3(0.0f, 1.0f, 0.0f)));
		setCamera();
	});

	dPress = new std::function<void(int)>([this, setCamera](int keyCode)
	{
		player->Translate(glm::vec3(-moveSpeed, 0.0f, 0.0f));
		setCamera();
	});

	setClipOnMoveRelease = new std::function<void(int)>([this, setCamera](int keyCode)
	{
		player->SetClip(5);
	});

	setClipOnMovePress = new std::function<void(int)>([this, setCamera](int keyCode)
	{
		player->SetClip(0);
	});
	
	setClipOnJumpPress = new std::function<void(int)>([this, setCamera](int keyCode)
	{
		player->SetClip(8);
	});

	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_W, wPress);
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_A, aPress);
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_S, sPress);
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_D, dPress);
	InputManager::RegisterCallbackForKeyState(KEY_RELEASE, KEY_W, setClipOnMoveRelease);
	InputManager::RegisterCallbackForKeyState(KEY_RELEASE, KEY_A, setClipOnMoveRelease);
	InputManager::RegisterCallbackForKeyState(KEY_RELEASE, KEY_S, setClipOnMoveRelease);
	InputManager::RegisterCallbackForKeyState(KEY_RELEASE, KEY_D, setClipOnMoveRelease);
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_W, setClipOnMovePress);
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_A, setClipOnMovePress);
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_S, setClipOnMovePress);
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_D, setClipOnMovePress);
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_SPACE, setClipOnJumpPress);
	InputManager::RegisterCallbackForKeyState(KEY_RELEASE, KEY_SPACE, setClipOnMoveRelease);
}

Player::~Player()
{
}

void Player::Update()
{
	

	
}
