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
#include "Renderer/GraphicsObjects/TexturedStaticGraphicsObject.h"
#include "UI/UserInterfaceItem.h"
#include "Input/InputManager.h"
#include "Renderer/Cameras/CameraManager.h"
#include "Renderer/Cameras/Camera.h"
#include "Math/Math.h"

#include "Time/TimeManager.h"

#include "Entities/Skybox.h"
#include "Entities/Player.h"

#include <cstdio>

class HeapProfiling
{
public:
	~HeapProfiling()
	{

	};
};

std::function<void(int)>* wPress = nullptr;
std::function<void(int)>* aPress = nullptr;
std::function<void(int)>* sPress = nullptr;
std::function<void(int)>* dPress = nullptr;
std::function<void(int)>* lctrPress = nullptr;
std::function<void(int)>* spacePress = nullptr;
std::function<void(int)>* qPress = nullptr;
std::function<void(int)>* ePress = nullptr;
std::function<void(int)>* lPress = nullptr;
std::function<void(int)>* cPress = nullptr;
std::function<void(int)>* zPress = nullptr;

std::function<void(int)>* tPress = nullptr;
std::function<void(int)>* yPress = nullptr;
std::function<void(int)>* uPress = nullptr;

static HeapProfiling heapProfiling;

void LoadAssets();
void Game();
void SetInput();
void AddGrid();



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
	TextureManager::LoadTexture("Assets/Textures/Woman.png", "Woman");
	TextureManager::LoadTexture("Assets/Textures/Woman1.png", "Woman1");
	TextureManager::LoadTexture("Assets/Textures/Woman2.png", "Woman2");
	TextureManager::LoadTexture("Assets/Textures/Woman3.png", "Woman3");
	TextureManager::LoadTexture("Assets/Textures/Cruiser.png", "Cruiser");
	TextureManager::LoadTexture("Assets/Textures/VikingRoom.png", "VikingRoom");
	TextureManager::LoadTexture("Assets/Textures/HumanStatic.png", "Human");
	TextureManager::LoadTexture("Assets/Textures/Skybox2.png", "Skybox");
	TextureManager::LoadTexture("Assets/Textures/Coco.png", "Coco");
	TextureManager::LoadTexture("Assets/Textures/grid.png", "Grid");
	TextureManager::LoadTexture("Assets/Textures/FeatureSearchToolBackground.png", "FeatureSearchToolBackground");

	ModelManager::LoadModel("Circle", "Assets/Models/Circle.gltf");
	ModelManager::LoadModel("Woman", "Assets/Models/Woman.gltf");
	ModelManager::LoadModel("Cruiser", "Assets/Models/Cruiser.gltf");
	ModelManager::LoadModel("CruiserAnim", "Assets/Models/CruiserAnim.gltf");
	ModelManager::LoadModel("Ball", "Assets/Models/NewBall.gltf");
	ModelManager::LoadModel("Cube", "Assets/Models/Cube.gltf");
	ModelManager::LoadModel("Skybox", "Assets/Models/Skybox.gltf");
	ModelManager::LoadModel("Human", "Assets/Models/Human.gltf");
	ModelManager::LoadModel("Home", "Assets/Models/HomeModel.gltf");
	ModelManager::LoadModel("Sphere", "Assets/Models/Sphere.gltf");
	ModelManager::LoadModel("VikingRoom", "Assets/Models/VikingRoom.gltf");
	ModelManager::LoadModel("Dragon", "Assets/Models/Dragon.gltf");
	ModelManager::LoadModel("Square", "Assets/Models/Square.gltf");
}

std::vector<std::function<void(int)>**> keyList = { &wPress, &aPress, &dPress, &lctrPress, &qPress, &ePress };

void Game()
{
	//StressTest();
	AddGrid();

	Scene* mainScene = SceneManager::CreateScene("Main");
	Skybox* skyboxGameObject = new Skybox();
	Player* playerGameObject = new Player();

	mainScene->AddEntity("Skybox", skyboxGameObject);
	mainScene->AddEntity("Player", playerGameObject);

	float frame = TimeManager::SecondsSinceStart();

	while (Engine::Operating())
	{
		float frameTime = 0.0016f;
		if ((TimeManager::SecondsSinceStart() - frame) > frameTime)
		{	
			skyboxGameObject->Update();
			playerGameObject->Update();
			frame = TimeManager::SecondsSinceStart();
		}
	};

	for (std::function<void(int)>** keyPressFunction : keyList)
	{
		if (*keyPressFunction != nullptr)
		{
			delete *keyPressFunction;
		}
	}

	delete playerGameObject;
	delete skyboxGameObject;
}

void SetInput()
{

	Camera& cam = CameraManager::GetCamera("MainCamera");

	wPress = new std::function<void(int)>([&cam](int keyCode)
	{
		float speed = 4.0f * TimeManager::DeltaTime();
		cam.Translate(cam.GetForwardVector() * speed);
	});

	aPress = new std::function<void(int)>([&cam](int keyCode)
	{
		float speed = 4.0f * TimeManager::DeltaTime();
		cam.Translate(cam.GetRightVector() * -speed);
	});

	sPress = new std::function<void(int)>([&cam](int keyCode)
	{
		float speed = 4.0f * TimeManager::DeltaTime();
		cam.Translate(cam.GetForwardVector() * -speed);
	});

	dPress = new std::function<void(int)>([&cam](int keyCode)
	{
		float speed = 4.0f * TimeManager::DeltaTime();
		cam.Translate(cam.GetRightVector() * speed);
	});

	lctrPress = new std::function<void(int)>([&cam](int keyCode)
	{
		float speed = 4.0f * TimeManager::DeltaTime();
		cam.Translate(glm::vec3(0.0f, -speed, 0.0f));
	});

	spacePress = new std::function<void(int)>([&cam](int keyCode)
	{
		float speed = 4.0f * TimeManager::DeltaTime();
		cam.Translate(glm::vec3(0.0f, speed, 0.0f));
	});

	qPress = new std::function<void(int)>([&cam](int keyCode)
	{
		float rotSpeed = 2.0f * TimeManager::DeltaTime();
		cam.Rotate(cam.GetUpVector(), rotSpeed);
	});

	ePress = new std::function<void(int)>([&cam](int keyCode)
	{
		float rotSpeed = 2.0f * TimeManager::DeltaTime();
		cam.Rotate(cam.GetUpVector(), -rotSpeed);
	});
	

	zPress = new std::function<void(int)>([&cam](int keyCode)
	{
		float rotSpeed = 2.0f * TimeManager::DeltaTime();
		cam.Rotate(cam.GetRightVector(), -rotSpeed);
	});

	cPress = new std::function<void(int)>([&cam](int keyCode)
	{
		float rotSpeed = 2.0f * TimeManager::DeltaTime();
		cam.Rotate(cam.GetRightVector(), rotSpeed);
	});



	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_L, lPress, "CameraMoveFromMain");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_C, cPress, "CameraMoveFromMain");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_Z, zPress, "CameraMoveFromMain");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_W, wPress, "CameraMoveFromMain");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_A, aPress, "CameraMoveFromMain");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_S, sPress, "CameraMoveFromMain");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_D, dPress, "CameraMoveFromMain");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_Q, qPress, "CameraMoveFromMain");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_E, ePress, "CameraMoveFromMain");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_LEFT_CTRL, lctrPress, "CameraMoveFromMain");
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_SPACE, spacePress, "CameraMoveFromMain");
}

void AddGrid()
{
	GraphicsObjectManager::CreateTexturedStaticGraphicsObject(ModelManager::GetModel("DefaultRectangleWithDepth"), TextureManager::GetTexture("Grid"),
		[](TexturedStaticGraphicsObject* go)
		{
			go->Rotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
			go->SetScale({ 50.0f, 50.0f, 1.0f });
			go->Translate({ 0.0f, -1.0f, 0.0f });
		});
}
