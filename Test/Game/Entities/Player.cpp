#include "Player.h"

#include "Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "Renderer/GraphicsObjects/TexturedAnimatedGraphicsObject.h"
#include "Renderer/GraphicsObjects/ColoredStaticGraphicsObject.h"
#include "Renderer/GraphicsObjects/LitTexturedStaticGraphicsObject.h"
#include "Renderer/Model/ModelManager.h"
#include "Renderer/Model/Model.h"
#include "Renderer/Cameras/CameraManager.h"
#include "Renderer/Cameras/Camera.h"
#include "Renderer/Images/TextureManager.h"
#include "Input/InputManager.h"
#include "Collision/BoundingSphere.h"
#include "Collision/OrientedBoundingBox.h"

#include "Animation/BakedAnimation.h"
#include "Animation/Animation.h";
#include "Animation/Armature.h"

Player::Player() :
	model(ModelManager::GetModel("Woman")),
	texture(TextureManager::GetTexture("Woman2")),
	graphics(nullptr),
	boundingSpheres(std::vector<OrientedBoundingBox*>()),
	collider(nullptr)
{
	GraphicsObjectManager::CreateTexturedAnimatedGraphicsObject(model, texture, [this](TexturedAnimatedGraphicsObject* obj)
		{
			graphics = obj;
		});

	std::vector<std::string> jointNames = model->GetArmature()->GetJointNames();

	for (unsigned int i = 0; i < jointNames.size(); i++)
	{
		std::vector<Vertex> vertices;
		for (const Vertex& vert : model->GetVertices())
		{
			glm::ivec4 influences = vert.GetInfluences();
			if (influences.x == i || influences.y == i || influences.z == i || influences.w == i)
			{
				vertices.push_back(vert);
			}
		}

		if(!vertices.empty())
			boundingSpheres.push_back(new OrientedBoundingBox([](Entity* owner) {}, this, vertices, glm::mat4(1.0f)));
	}
	
	collider = new OrientedBoundingBox([](Entity* owner) {}, this, model->GetVertices(), glm::mat4(1.0f));

	GraphicsObjectManager::CreateTexturedAnimatedGraphicsObject(model, texture, [this](TexturedAnimatedGraphicsObject* obj) 
		{ 
			obj->SetClip(9); 

			obj->GetModel()->GetBakedAnimation(9);
		});



	RegisterInput();
}

Player::~Player()
{
	delete wPress;
	delete wRelease;
	delete collider;
	delete wPressed;

	for (OrientedBoundingBox* sphere : boundingSpheres)
	{
		delete sphere;
	}
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

		cam.SetPosition(cruiserPosition + (cruiserUp * 6.0f) + (-cruiserForward * 6.0f));
		cam.SetTarget(cruiserPosition + (cruiserForward * 3.0f));

		for (OrientedBoundingBox* sphere : boundingSpheres)
		{
			sphere->ComputeData(model->GetVertices(), glm::mat4(1.0f));
		}

		collider->ComputeData(model->GetVertices(), graphics->GetTransform());


		InputManager::WhenCursorMoved([this, cruiserUp, cruiserRight, cruiserForward](const glm::vec2& newPosition)
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
				//cruiserGraphics->Rotate(moveSpeed * yDif * upDown, worldRight);

				lastPosition = newPosition;
			});

		//if (centerCollisionBoundingSphere->Intersect(*collider))
		//{
		//	centerCollisionBoundingSphere->SetColor({ 1.0f, 0.0f, 0.0f, 1.0f });
		//}
		//else
		//{
		//	centerCollisionBoundingSphere->SetColor({ 0.0f, 0.5f, 0.5f, 1.0f });
		//}
	}
}

void Player::RegisterInput()
{
	
	wPressed = new std::function<void(int)>([this](int keyCode)
		{
			glm::mat4 cruiserTransform = graphics->GetTransform();
			glm::vec3 cruiserForward = glm::normalize(cruiserTransform[2]);
			graphics->Translate(cruiserForward * 0.02f);
		});

	wPress = new std::function<void(int)>([this](int keyCode)
		{
			graphics->SetClip(0);
		});

	wRelease = new std::function<void(int)>([this](int keyCode)
		{
			graphics->SetClip(4);
		});
	
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_W, wPressed, "playerForward");
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_W, wPress, "playerRunAnimation");
	InputManager::RegisterCallbackForKeyState(KEY_RELEASE, KEY_W, wRelease, "playerIdleAnimation");

}
