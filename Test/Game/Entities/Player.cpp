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
	wPressed(nullptr),
	aPressed(nullptr),
	sPressed(nullptr),
	dPressed(nullptr),
	setClipOnMoveRelease(nullptr),
	setClipOnJumpPress(nullptr),
	moveSpeed(0.005f),
	movementInProgress(false)
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
			cam.SetPosition(translation + glm::vec3(0.0f, 10.0f, -15.0f));
		}
	};

	wPressed = new std::function<void(int)>([this, setCamera](int keyCode)
	{
		if(movementInProgress && direction == 0)
		{
			player->Translate(glm::vec3(0.0f, 0.0f, moveSpeed));
			player->SetRotation(glm::mat4(1.0f));
			setCamera();
		}
	});

	aPressed = new std::function<void(int)>([this, setCamera](int keyCode)
	{
		if (movementInProgress && direction == 1)
		{
			player->Translate(glm::vec3(moveSpeed, 0.0f, 0.0f));
			setCamera();
		}
	});

	sPressed = new std::function<void(int)>([this, setCamera](int keyCode)
	{
		if (movementInProgress and direction == 2)
		{
			player->Translate(glm::vec3(0.0f, 0.0f, -moveSpeed));
			setCamera();
		}
	});

	dPressed = new std::function<void(int)>([this, setCamera](int keyCode)
	{
		if (movementInProgress and direction == 3)
		{
			player->Translate(glm::vec3(-moveSpeed, 0.0f, 0.0f));
			setCamera();
		}
	});

	wPress = new std::function<void(int)>([this, setCamera](int keyCode)
	{
		if(!movementInProgress)
		{
			player->SetClip(0);
			player->SetRotation(glm::mat4(1.0f));
			setCamera();
			movementInProgress = true;
			direction = 0;
		}
	});

	aPress = new std::function<void(int)>([this, setCamera](int keyCode)
	{
		if(!movementInProgress)
		{
			player->SetClip(0);
			player->SetRotation(glm::rotate(glm::mat4(1.0f), 1.57f, glm::vec3(0.0f, 1.0f, 0.0f)));
			movementInProgress = true;
			direction = 1;
		}
	});

	sPress = new std::function<void(int)>([this, setCamera](int keyCode)
	{
		if(!movementInProgress)
		{
			player->SetClip(0);
			player->SetRotation(glm::rotate(glm::mat4(1.0f), 3.1415f, glm::vec3(0.0f, 1.0f, 0.0f)));
			movementInProgress = true;
			direction = 2;
		}
	});

	dPress = new std::function<void(int)>([this, setCamera](int keyCode)
	{
		if(!movementInProgress)
		{
			player->SetRotation(glm::rotate(glm::mat4(1.0f), -1.57f, glm::vec3(0.0f, 1.0f, 0.0f)));
			player->SetClip(0);
			movementInProgress = true;
			direction = 3;
		}
	});

	setClipOnMoveRelease = new std::function<void(int)>([this, setCamera](int keyCode)
	{
		player->SetClip(4);

		movementInProgress = false;
	});

	setClipOnJumpPress = new std::function<void(int)>([this, setCamera](int keyCode)
	{
		if (!movementInProgress)
		{
			player->SetClip(8);
		}
	});

	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_W, wPress, "CharacterMovement");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_A, aPress, "CharacterMovement");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_S, sPress, "CharacterMovement");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_D, dPress, "CharacterMovement");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_W, wPressed, "CharacterMovement");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_A, aPressed, "CharacterMovement");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_S, sPressed, "CharacterMovement");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_D, dPressed, "CharacterMovement");
	InputManager::RegisterCallbackForKeyState(KEY_RELEASE, KEY_W, setClipOnMoveRelease, "CharacterMovement");
	InputManager::RegisterCallbackForKeyState(KEY_RELEASE, KEY_A, setClipOnMoveRelease, "CharacterMovement");
	InputManager::RegisterCallbackForKeyState(KEY_RELEASE, KEY_S, setClipOnMoveRelease, "CharacterMovement");
	InputManager::RegisterCallbackForKeyState(KEY_RELEASE, KEY_D, setClipOnMoveRelease, "CharacterMovement");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_SPACE, setClipOnJumpPress, "CharacterMovement");
	InputManager::RegisterCallbackForKeyState(KEY_RELEASE, KEY_SPACE, setClipOnMoveRelease, "CharacterMovement");
}

Player::~Player()
{
	delete wPress;
	delete aPress;
	delete sPress;
	delete dPress;

	delete wPressed;
	delete aPressed;
	delete sPressed;
	delete dPressed;
	
	delete setClipOnMoveRelease;
	
	delete setClipOnJumpPress;
}

void Player::Update()
{
	

	
}
