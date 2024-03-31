#include "Player.h"

#include "Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "Renderer/GraphicsObjects/TexturedAnimatedGraphicsObject.h"
#include "Renderer/GraphicsObjects/ColoredStaticGraphicsObject.h"
#include "Renderer/Model/ModelManager.h"
#include "Renderer/Cameras/CameraManager.h"
#include "Renderer/Cameras/Camera.h"
#include "Renderer/Images/TextureManager.h"
#include "Input/InputManager.h"
#include "Collision/BoundingSphere.h"
#include "Collision/AxisAlignedBoundingBox.h"

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
	model = ModelManager::GetModel("Woman");
	texture = TextureManager::GetTexture("Woman2");

	auto onGraphicsObjectCreate = [this](TexturedAnimatedGraphicsObject* go)
		{
			graphics = go;
			graphics->SetClip(0);
			graphics->SetAnimationSpeed(1.0f);
			graphics->Scale({2.0f, 2.0f, 2.0f});
	};

	GraphicsObjectManager::CreateTexturedAnimatedGraphicsObject(model, texture, onGraphicsObjectCreate);

	RegisterInput();

	collisionVolume = new BoundingSphere([](Entity* owner) {}, this, model, glm::mat4(1.0f));
	collisionVolume->Initialize(model);


	glm::mat4 collisionTransform = glm::mat4(1.0f);

	collisionTransform = glm::scale(collisionTransform, { 2.0f, 2.0f, 2.0f });

	randomCollisionSphere = new BoundingSphere([](Entity* owner) {}, this, ModelManager::GetModel("Human"), collisionTransform);

	randomCollisionBox = new AxisAlignedBoundingBox([](Entity*) {}, this, ModelManager::GetModel("Human"), collisionTransform);

	GraphicsObjectManager::CreateColoredStaticGraphicsObject(ModelManager::GetModel("Human"), { 0.0f, 0.0f, 0.0f, 1.0f }, [this](ColoredStaticGraphicsObject* go) { testObj = go;  go->Scale({ 2.0f, 2.0f, 2.0f }); });


}

Player::~Player()
{
	delete collisionVolume;
	delete randomCollisionSphere;
	delete randomCollisionBox;

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
	if (graphics != nullptr)
	{
		collisionVolume->ComputeData(model, graphics->GetTransform());

		if (collisionVolume->Intersect(*randomCollisionSphere))
		{
			collisionVolume->SetColor({ 1.0f, 0.0f, 0.0f, 1.0f });
		}
		else
		{
			collisionVolume->SetColor({ 0.0f, 0.5f, 0.5f, 1.0f });
		}
	}

	if(testObj != nullptr)
		randomCollisionSphere->ComputeData(ModelManager::GetModel("Human"), testObj->GetTransform());
}

void Player::RegisterInput()
{
	auto setCamera = [this]()
		{
			Camera& cam = CameraManager::GetCamera("MainCamera");

			if (graphics != nullptr)
			{
				glm::vec3 translation = graphics->GetTranslation();
				cam.SetTarget(translation);
				//cam.SetPosition(translation + glm::vec3(0.0f, 30.0f, -1.0f));
			}
		};

	wPressed = new std::function<void(int)>([this, setCamera](int keyCode)
		{
			if (movementInProgress && direction == 0)
			{
				graphics->Translate(glm::vec3(0.0f, 0.0f, moveSpeed));
				graphics->SetRotation(glm::mat4(1.0f));
				setCamera();
			}
		});

	aPressed = new std::function<void(int)>([this, setCamera](int keyCode)
		{
			if (movementInProgress && direction == 1)
			{
				graphics->Translate(glm::vec3(moveSpeed, 0.0f, 0.0f));
				setCamera();
			}
		});

	sPressed = new std::function<void(int)>([this, setCamera](int keyCode)
		{
			if (movementInProgress and direction == 2)
			{
				graphics->Translate(glm::vec3(0.0f, 0.0f, -moveSpeed));
				setCamera();
			}
		});

	dPressed = new std::function<void(int)>([this, setCamera](int keyCode)
		{
			if (movementInProgress and direction == 3)
			{
				graphics->Translate(glm::vec3(-moveSpeed, 0.0f, 0.0f));
				setCamera();
			}
		});

	wPress = new std::function<void(int)>([this, setCamera](int keyCode)
		{
			if (!movementInProgress)
			{
				graphics->SetClip(0);
				graphics->SetRotation(glm::mat4(1.0f));
				setCamera();
				movementInProgress = true;
				direction = 0;
			}
		});

	aPress = new std::function<void(int)>([this, setCamera](int keyCode)
		{
			if (!movementInProgress)
			{
				graphics->SetClip(0);
				graphics->SetRotation(glm::rotate(glm::mat4(1.0f), 1.57f, glm::vec3(0.0f, 1.0f, 0.0f)));
				movementInProgress = true;
				direction = 1;
			}
		});

	sPress = new std::function<void(int)>([this, setCamera](int keyCode)
		{
			if (!movementInProgress)
			{
				graphics->SetClip(0);
				graphics->SetRotation(glm::rotate(glm::mat4(1.0f), 3.1415f, glm::vec3(0.0f, 1.0f, 0.0f)));
				movementInProgress = true;
				direction = 2;
			}
		});

	dPress = new std::function<void(int)>([this, setCamera](int keyCode)
		{
			if (!movementInProgress)
			{
				graphics->SetRotation(glm::rotate(glm::mat4(1.0f), -1.57f, glm::vec3(0.0f, 1.0f, 0.0f)));
				graphics->SetClip(0);
				movementInProgress = true;
				direction = 3;
			}
		});

	setClipOnMoveRelease = new std::function<void(int)>([this, setCamera](int keyCode)
		{
			graphics->SetClip(4);

			movementInProgress = false;
		});

	setClipOnJumpPress = new std::function<void(int)>([this, setCamera](int keyCode)
		{
			if (!movementInProgress)
			{
				graphics->SetClip(8);
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
