#include "Player.h"

#include "Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "Renderer/GraphicsObjects/TexturedAnimatedGraphicsObject.h"
#include "Renderer/GraphicsObjects/ColoredStaticGraphicsObject.h"
#include "Renderer/GraphicsObjects/LitTexturedStaticGraphicsObject.h"
#include "Renderer/Model/ModelManager.h"
#include "Renderer/Model/Model.h"
#include "Renderer/Windows/WindowManager.h"
#include "Renderer/Windows/Window.h"
#include "Renderer/Cameras/CameraManager.h"
#include "Renderer/Cameras/Camera.h"
#include "Renderer/Images/TextureManager.h"
#include "Input/InputManager.h"
#include "Collision/BoundingSphere.h"
#include "Collision/AxisAlignedBoundingBox.h"
#include "Collision/OrientedBoundingBox.h"
#include "Renderer/GraphicsObjects/TexturedStatic2DGraphicsObject.h"

#include "Math/Shapes/Circle.h"
#include "Math/Shapes/Rectangle.h"
#include "Math/Shapes/OrientedRectangle.h"
#include "UI/UserInterfaceManager.h"
#include "UI/UserInterfaceItem.h"

#include "Animation/BakedAnimation.h"
#include "Animation/Animation.h";
#include "Animation/Armature.h"

Player::Player() :
	model(ModelManager::GetModel("Woman")),
	texture(TextureManager::GetTexture("Woman2")),
	graphics(nullptr),
	hoveredPress(false)
{
	GraphicsObjectManager::CreateTexturedAnimatedGraphicsObject(model, texture, [this](TexturedAnimatedGraphicsObject* obj)
		{
			graphics = obj;

		});

	RegisterInput();


	collisionObj2D = UserInterfaceManager::CrateUserInterfaceItem("Square0", ModelManager::GetModel("Square"), TextureManager::GetTexture("DefaultFontTexture"));
	
	collisionObj2D->TransformReady([this]()
		{
			collisionObj2D->Scale(100.0f, 50.0f);
			collisionObj2D->Rotate(45.0f);
		});

	float angleInRadians = -45.0f / 57.2957795f;
	collider2D = new OrientedRectangle({ 100.0f, 100.0f }, { 50.0f, 25.0f }, angleInRadians);

	collider2DOther = new OrientedRectangle({ 400.0f, 200.0f }, {50.0f, 25.0f}, angleInRadians);

	collisionObj2DOther = UserInterfaceManager::CrateUserInterfaceItem("Square", ModelManager::GetModel("Square"), TextureManager::GetTexture("DefaultFontTexture"));
	
	collisionObj2DOther->TransformReady([this]()
		{
			collisionObj2DOther->Scale(100.0f, 50.0f);
			collisionObj2DOther->SetPosition(350.0f, 150.0f);
			collisionObj2DOther->Rotate(45.0f);
		});
}

Player::~Player()
{
	delete wPress;
	delete wRelease;
	delete wPressed;
	delete iPress;
	delete iRelease;
}

void Player::Update()
{
	if (graphics != nullptr)
	{
		Camera& cam = CameraManager::GetActiveCamera();

		glm::mat4 cruiserTransform = graphics->GetTransform();

		glm::vec3 cruiserPosition = cruiserTransform[3];
		glm::vec3 cruiserUp = glm::normalize(cruiserTransform[1]);
		glm::vec3 cruiserRight = glm::normalize(cruiserTransform[0]);
		glm::vec3 cruiserForward = glm::normalize(cruiserTransform[2]);

		cam.SetPosition(cruiserPosition + (cruiserUp * 10.0f) + (-cruiserForward * 10.0f));
		cam.SetTarget(cruiserPosition + (cruiserForward * 3.0f));

		InputManager::WhenCursorMoved([this, cruiserUp, cruiserRight, cruiserForward](const glm::vec2& newPosition)
			{
				static bool lastCall = true;

				if (lastCall)
				{
					static float moveSpeed = 0.1f;

				static glm::vec2 lastPosition = newPosition;

				float xDif = glm::max(lastPosition.x, newPosition.x) - glm::min(lastPosition.x, newPosition.x);
				float yDif = glm::max(lastPosition.y, newPosition.y) - glm::min(lastPosition.y, newPosition.y);

				float leftRight = (lastPosition.x > newPosition.x) ? 1.0f : -1.0f;
				float upDown = (lastPosition.y > newPosition.y) ? 1.0f : -1.0f;

				glm::mat4 cruiserRotation(1.0f);
				cruiserRotation[0] = glm::vec4(cruiserRight, 0.0f);
				cruiserRotation[1] = glm::vec4(cruiserUp, 0.0f);
				cruiserRotation[2] = glm::vec4(cruiserForward, 0.0f);
				cruiserRotation[2] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

				glm::vec3 worldRight = glm::normalize(glm::cross(cruiserUp, cruiserForward));
				glm::vec3 worldUp = glm::normalize(glm::cross(cruiserForward, cruiserRight));

				graphics->Rotate(moveSpeed * xDif * leftRight, worldUp);

				lastPosition = newPosition;

				}
			});

		static glm::vec2 cursorPos;
		InputManager::GetCursorPosition([this](const glm::vec2& pos)
			{
				std::function<void()> hovered = [this, pos]()
					{
						if (hoveredPress)
						{
							Window* mainWindow = WindowManager::GetWindow("MainWindow");
							glm::vec2 cursorPosition = pos;
							float windowHeight = static_cast<float>(mainWindow->GetHeight());
							cursorPosition.y = windowHeight - cursorPosition.y;
							collisionObj2D->SetPosition(pos.x, cursorPosition.y);

							collider2D->SetPosition({ pos.x + 50.0f, cursorPosition.y + 50.0f });

							if (collider2D->OrientedRectangleIntersect(*collider2DOther))
							{
								GraphicsObjectManager::WireFrame(collisionObj2D->GetGraphicsObject(), ObjectTypes::GraphicsObjectType::TexturedStatic2D);
							}
							else
							{
								GraphicsObjectManager::Solid(collisionObj2D->GetGraphicsObject(), ObjectTypes::GraphicsObjectType::TexturedStatic2D);
							}
						}
					};
				collisionObj2D->Hovered(hovered);
			});
	}
}

void Player::RegisterInput()
{
	
	wPressed = new std::function<void(int)>([this](int keyCode)
		{
			glm::mat4 cruiserTransform = graphics->GetTransform();
			glm::vec3 cruiserForward = glm::normalize(cruiserTransform[2]);
			graphics->Translate(cruiserForward * 0.002f);
		});

	wPress = new std::function<void(int)>([this](int keyCode)
		{
			graphics->SetClip(0);
		});

	wRelease = new std::function<void(int)>([this](int keyCode)
		{
			graphics->SetClip(4);
		});

	iRelease = new std::function<void(int)>([this](int keyCode)
		{
			hoveredPress = false;
		});

	iPress = new std::function<void(int)>([this](int keyCode)
		{
			hoveredPress = true;
		});
	
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_W, wPressed, "playerForward");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_W, wPress, "playerRunAnimation");
	InputManager::RegisterCallbackForKeyState(KEY_RELEASE, KEY_W, wRelease, "playerIdleAnimation");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_I, iPress, "circleMovement");
	InputManager::RegisterCallbackForKeyState(KEY_RELEASE, KEY_I, iRelease, "circleMovement");

}
