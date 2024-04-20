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
#include "Math/Shapes/Sphere.h"
#include "Math/Shapes/AxisAlignedBoundingBox.h"
#include "Math/Shapes/OrientedBoundingBox.h"
#include "Renderer/GraphicsObjects/TexturedStatic2DGraphicsObject.h"

#include "Collision/OrientedBoundingBoxWithVisualization.h"
#include "Collision/AnimatedCollider.h"
#include "Math/Shapes/Circle.h"
#include "Math/Shapes/Rectangle.h"
#include "Math/Shapes/OrientedRectangle.h"
#include "UI/UserInterfaceManager.h"
#include "UI/UserInterfaceItem.h"

#include "Animation/BakedAnimation.h"
#include "Animation/Animation.h"
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

			otherObb = new OrientedBoundingBoxWithVisualization(graphics->GetModel()->GetVertices());
			collider = new AnimatedCollider(graphics);
		});
	
	RegisterInput();
}

Player::~Player()
{
	delete wPress;
	delete wRelease;
	delete wPressed;
	delete iPress;

	delete otherObb;
}

void Player::Update()
{
	CameraAndPlayerRotation();

	if (collider)
	{
		collider->Update();
		collider->Intersect(*otherObb);
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

	iPress = new std::function<void(int)>([this](int keyCode)
		{
			collider->ToggleVisibility();
		});
	
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_W, wPressed, "playerForward");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_W, wPress, "playerRunAnimation");
	InputManager::RegisterCallbackForKeyState(KEY_RELEASE, KEY_W, wRelease, "playerIdleAnimation");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_I, iPress, "circleMovement");

}

void Player::CameraAndPlayerRotation()
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

		
	}
}
