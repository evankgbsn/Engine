#include "Engine.h"

#include "Scene/SceneManager.h"
#include "Scene/Scene.h"
#include "Entity/GameObject.h"
#include "System/TransformSystem.h"
#include "Component/TransformComponent.h"
#include "Renderer/Images/TextureManager.h"
#include "Renderer/Model/ModelManager.h"
#include "Renderer/Model/Model.h"
#include "Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "Renderer/GraphicsObjects/TexturedAnimatedGraphicsObject.h"
#include "Renderer/GraphicsObjects/TexturedStatic2DGraphicsObject.h"
#include "Renderer/GraphicsObjects/TexturedStaticGraphicsObject.h"
#include "UI/UserInterfaceItem.h"
#include "Input/InputManager.h"
#include "Renderer/Cameras/CameraManager.h"
#include "Renderer/Cameras/Camera.h"

#include "Time/TimeManager.h"

#include "Entities/Skybox.h"

#include <cstdio>

class HeapProfiling
{
public:
	~HeapProfiling()
	{

	};
};

std::function<void()>* wPress = nullptr;
std::function<void()>* aPress = nullptr;
std::function<void()>* sPress = nullptr;
std::function<void()>* dPress = nullptr;
std::function<void()>* lctrPress = nullptr;
std::function<void()>* spacePress = nullptr;
std::function<void()>* qPress = nullptr;
std::function<void()>* ePress = nullptr;

static HeapProfiling heapProfiling;

void StressTest();
void LoadAssets();
void Game();
void SetInput();

int main(int argc, const char** argv)
{
	const std::string gameName("Test");
	const Engine::Version gameVersion(1, 0, 0);

	Engine::Initialize(gameName, gameVersion);
	Engine::SetLoadAssetsFunc(&LoadAssets);
	Engine::SetGameThreadFunc(&Game);
	Engine::Start();
	Engine::Terminate();
	return 0;
}

void LoadAssets()
{
	TextureManager::LoadTexture("../Engine/Engine/Renderer/Images/Woman.png", "Woman");
	TextureManager::LoadTexture("../Engine/Engine/Renderer/Images/Woman1.png", "Woman1");
	TextureManager::LoadTexture("../Engine/Engine/Renderer/Images/Woman2.png", "Woman2");
	TextureManager::LoadTexture("../Engine/Engine/Renderer/Images/Woman3.png", "Woman3");
	TextureManager::LoadTexture("../Engine/Engine/Renderer/Images/Cruiser.png", "Cruiser");
	TextureManager::LoadTexture("../Engine/Engine/Renderer/Images/VikingRoom.png", "VikingRoom");
	TextureManager::LoadTexture("../Engine/Engine/Renderer/Images/HumanStatic.png", "Human");
	TextureManager::LoadTexture("../Engine/Engine/Renderer/Images/Skybox2.png", "Skybox");

	ModelManager::LoadModel("Woman", "../Engine/Engine/Renderer/Model/Woman.gltf");
	ModelManager::LoadModel("Cruiser", "../Engine/Engine/Renderer/Model/Cruiser.gltf");
	ModelManager::LoadModel("CruiserAnim", "../Engine/Engine/Renderer/Model/CruiserAnim.gltf");
	ModelManager::LoadModel("Ball", "../Engine/Engine/Renderer/Model/NewBall.gltf");
	ModelManager::LoadModel("Human", "../Engine/Engine/Renderer/Model/Human.gltf");
	ModelManager::LoadModel("Cube", "../Engine/Engine/Renderer/Model/Cube.gltf");
	ModelManager::LoadModel("Skybox", "../Engine/Engine/Renderer/Model/Skybox.gltf");

}

void StressTest()
{
	Texture* womanTextures[5] = { 
		TextureManager::GetTexture("Woman"), 
		TextureManager::GetTexture("Woman1"), 
		TextureManager::GetTexture("Woman2"), 
		TextureManager::GetTexture("Woman3"), 
		TextureManager::GetTexture("Woman")
	};

	const float translationScalar = 7.0f;
	srand(static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
	unsigned int clipNum = 0;
	unsigned int womanTextureNum;

	for (unsigned int i = 0; i < 10; i++)
	{
		for (unsigned int j = 0; j < 10; j++)
		{
			for (unsigned int k = 0; k < 10; k++)
			{
				if (clipNum > 9)
					clipNum = 0;
	
				womanTextureNum = rand() % 5;

				auto animationCreationCallback = [translationScalar, i, j, k](GraphicsObject* go)
				{
					TexturedAnimatedGraphicsObject* ago = static_cast<TexturedAnimatedGraphicsObject*>(go);
					if (go != nullptr)
					{
						ago->Translate(glm::vec3(i * translationScalar, j * translationScalar, k * translationScalar));
						ago->SetClip(0);
						ago->SetAnimationSpeed(1.0f);
					}

					if (i == j)
					{
						GraphicsObjectManager::WireFrame(go, ObjectTypes::GraphicsObjectType::AnimatedTextured);
					}

				};
				//std::this_thread::sleep_for(std::chrono::milliseconds(10));
				GraphicsObjectManager::CreateTexturedAnimatedGraphicsObject(ModelManager::GetModel("Cube"), TextureManager::GetTexture("Woman"), animationCreationCallback);
				
			}
		}
	}

	auto rectangleCallback = [](GraphicsObject* obj)
	{
		TexturedStatic2DGraphicsObject* staticobj = static_cast<TexturedStatic2DGraphicsObject*>(obj);

		if (obj != nullptr)
		{
			//TRS maybe queue these.
			staticobj->Translate({ -500.0f, -500.f });
			staticobj->Scale({ 500.f, 500.f});
		}
	};
	
	UserInterfaceItem* uiItem = new UserInterfaceItem(ModelManager::GetModel("DefaultRectangle"), TextureManager::GetTexture("Woman"));

	//GraphicsObjectManager::CreateTexturedStatic2DGraphicsObject(ModelManager::GetModel("Cube"), TextureManager::GetTexture("VikingRoom"), rectangleCallback);
	//GraphicsObjectManager::CreateTexturedStatic2DGraphicsObject(ModelManager::GetModel("DefaultRectangle"), TextureManager::GetTexture("Human"), rectangleCallback);
	//GraphicsObjectManager::CreateGoochGraphicsObject(ModelManager::GetModel("Cruiser"), TextureManager::GetTexture("Woman"));
	//GraphicsObjectManager::CreateTexturedAnimatedGraphicsObject(ModelManager::GetModel("CruiserAnim"), TextureManager::GetTexture("Cruiser"));

	SetInput();
}

std::vector<std::function<void()>**> keyList = { &wPress, &aPress, &dPress, &lctrPress, &qPress, &ePress };

void Game()
{
	StressTest();

	Scene* mainScene = SceneManager::CreateScene("Main");
	TransformSystem* transformSystem = new TransformSystem();
	Skybox* skyboxGameObject = new Skybox();

	TransformComponent* skyboxTransform = transformSystem->CreateComponent();
	
	skyboxGameObject->AddComponent(skyboxTransform->GetType(), skyboxTransform);
	mainScene->AddEntity("Skybox", skyboxGameObject);
	mainScene->AddSystem("Transform", transformSystem);
	
	float frame = TimeManager::SecondsSinceStart();
	while (Engine::Operating())
	{
		if ((TimeManager::SecondsSinceStart() - frame) > 0.0041666f)
		{
			transformSystem->Operate();
			skyboxGameObject->Update();
			frame = TimeManager::SecondsSinceStart();
		}
	};

	for (std::function<void()>** keyPressFunction : keyList)
	{
		if (*keyPressFunction != nullptr)
		{
			delete *keyPressFunction;
		}
	}

	delete transformSystem;
	delete skyboxGameObject;

}

void SetInput()
{
	Camera& cam = CameraManager::GetCamera("MainCamera");

	float speed = 0.004f * TimeManager::DeltaTime();
	float rotSpeed = 0.002f *TimeManager::DeltaTime();

	wPress = new std::function<void()>([&cam, &speed]()
	{
		cam.Translate(cam.GetForwardVector() * speed);
	});

	aPress = new std::function<void()>([&cam, &speed]()
	{
		cam.Translate(cam.GetRightVector() * -speed);
	});

	sPress = new std::function<void()>([&cam, &speed]()
	{
		cam.Translate(cam.GetForwardVector() * -speed);
	});

	dPress = new std::function<void()>([&cam, &speed]()
	{
		cam.Translate(cam.GetRightVector() * speed);
	});

	lctrPress = new std::function<void()>([&cam, &speed]()
	{
		cam.Translate(glm::vec3(0.0f, -speed, 0.0f));
	});

	spacePress = new std::function<void()>([&cam, &speed]()
	{
		cam.Translate(glm::vec3(0.0f, speed, 0.0f));
	});

	qPress = new std::function<void()>([&cam, &rotSpeed]()
	{
		cam.Rotate(cam.GetUpVector(), rotSpeed);
	});

	ePress = new std::function<void()>([&cam, &rotSpeed]()
	{
		cam.Rotate(cam.GetUpVector(), -rotSpeed);
	});

	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_W, wPress);
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_A, aPress);
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_S, sPress);
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_D, dPress);
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_Q, qPress);
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_E, ePress);
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_LEFT_CTRL, lctrPress);
	InputManager::RegisterCallbackForKeyState(KEY_PRESS, KEY_SPACE, spacePress);
}