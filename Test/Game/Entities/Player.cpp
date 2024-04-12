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

		});

	std::unordered_map<std::string, std::vector<Vertex>> jointsVertices;

	const Armature* const arms = model->GetArmature();

	const std::vector<std::string>& jointNames = arms->GetJointNames();

	const std::vector<Vertex>& vertices = model->GetVertices();

	for (const Vertex& vert : vertices)
	{
		unsigned int jointWithMostInfluence = 0;
		float influence = vert.GetWeights()[jointWithMostInfluence];
		for (unsigned int i = 1; i < 4; ++i)
		{
			if (vert.GetWeights()[i] > influence)
			{
				jointWithMostInfluence = i;
				influence = vert.GetWeights()[i];
			}
		}

		jointsVertices[jointNames[vert.GetInfluences()[jointWithMostInfluence]]].push_back(vert);
	}

	for (const std::pair<std::string, std::vector<Vertex>>& jointVerts : jointsVertices)
	{
		OBB* newObb = new OBB();
		newObb->obb = new OrientedBoundingBox(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::mat4(1.0f));
		newObb->obb->SizeToMesh(jointVerts.second);

		for (unsigned int i = 0; i < jointNames.size(); ++i)
		{
			if (jointNames[i] == jointVerts.first)
			{
				newObb->jointIndex = i;
				break;
			}
		}

		GraphicsObjectManager::CreateColoredStaticGraphicsObject(ModelManager::GetModel("Cube"), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), [newObb, &jointVerts, this](ColoredStaticGraphicsObject* go)
			{
				newObb->graphics = go;
				go->SetScale(newObb->obb->GetSize());
				go->SetTranslation(newObb->obb->GetOffset());
				GraphicsObjectManager::WireFrame(go, ObjectTypes::GraphicsObjectType::ColoredStatic);
			});

		obbs.push_back(newObb);
	}

	otherObb = new OBB();

	otherObb->obb = new OrientedBoundingBox(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::mat4(1.0f));
	otherObb->obb->SizeToMesh(model->GetVertices());

	GraphicsObjectManager::CreateColoredStaticGraphicsObject(ModelManager::GetModel("Cube"), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), [this](ColoredStaticGraphicsObject* go)
		{
			otherObb->graphics = go;
			go->SetScale(otherObb->obb->GetSize());
			go->SetTranslation(otherObb->obb->GetOffset());
			otherObb->obb->SetOrigin(otherObb->obb->GetOffset());
			GraphicsObjectManager::WireFrame(go, ObjectTypes::GraphicsObjectType::ColoredStatic);
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

	for (const auto& obb : obbs)
	{
		delete obb->obb;
		delete obb;
	}

	delete otherObb->obb;
	delete otherObb;
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

		const Armature* const arms = model->GetArmature();

		for (OBB* obb : obbs)
		{
			if (obb->graphics != nullptr)
			{
				glm::mat4 scale(1.0f);
				scale = glm::scale(scale, obb->obb->GetSize());

				glm::mat4 translation(1.0f);
				translation = glm::translate(translation, obb->obb->GetOffset());

				obb->graphics->SetTransform(graphics->GetTransform()  * graphics->GetAnimPoseArray()[obb->jointIndex] * graphics->GetAnimInvBindPoseArray()[obb->jointIndex] * translation * scale);

				obb->obb->SetOrigin(obb->graphics->GetTranslation());

				if (obb->obb->OrientedBoundingBoxIntersect(*otherObb->obb))
				{
					obb->graphics->SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
				}
				else
				{
					obb->graphics->SetColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
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
