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

	GraphicsObjectManager::CreateColoredStaticGraphicsObject(ModelManager::GetModel("Cube"), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), [this](ColoredStaticGraphicsObject* go)
		{
			GraphicsObjectManager::WireFrame(go, ObjectTypes::GraphicsObjectType::ColoredStatic);
			
			obbGraphics = go;

			obb = new OrientedBoundingBox(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), go->GetRotation());
		});


	GraphicsObjectManager::CreateColoredStaticGraphicsObject(ModelManager::GetModel("Cube"), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), [this](ColoredStaticGraphicsObject* go)
		{
			GraphicsObjectManager::WireFrame(go, ObjectTypes::GraphicsObjectType::ColoredStatic);

			obbOtherGraphics = go;

			obbOther = new OrientedBoundingBox(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), go->GetRotation());
		});

	RegisterInput();
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

		if (obbGraphics != nullptr)
		{
			obbGraphics->SetTranslation(graphics->GetTranslation());
			obbGraphics->SetRotation(graphics->GetRotation());
			obb->SetOrientation(graphics->GetRotation());
			obb->SetOrigin(graphics->GetTranslation());

			if (obbOther != nullptr)
			{
				if (obb->OrientedBoundingBoxIntersect(*obbOther))
				{
					obbGraphics->SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
				}
				else
				{
					obbGraphics->SetColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
				}
			}
		}
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
