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
#include "Math/Math.h"

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

std::function<void(int)>* wPress = nullptr;
std::function<void(int)>* aPress = nullptr;
std::function<void(int)>* sPress = nullptr;
std::function<void(int)>* dPress = nullptr;
std::function<void(int)>* lctrPress = nullptr;
std::function<void(int)>* spacePress = nullptr;
std::function<void(int)>* qPress = nullptr;
std::function<void(int)>* ePress = nullptr;
std::function<void(int)>* lPress = nullptr;

std::function<void(int)>* tPress = nullptr;
std::function<void(int)>* yPress = nullptr;
std::function<void(int)>* uPress = nullptr;

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
	TextureManager::LoadTexture("../Engine/Engine/Renderer/Images/Coco.png", "Coco");
	TextureManager::LoadTexture("../Engine/Engine/Renderer/Images/grid.png", "Grid");
	TextureManager::LoadTexture("../Engine/Engine/Renderer/Images/FeatureSearchToolBackground.png", "FeatureSearchToolBackground");

	ModelManager::LoadModel("Woman", "../Engine/Engine/Renderer/Model/Woman.gltf");
	ModelManager::LoadModel("Cruiser", "../Engine/Engine/Renderer/Model/Cruiser.gltf");
	ModelManager::LoadModel("CruiserAnim", "../Engine/Engine/Renderer/Model/CruiserAnim.gltf");
	ModelManager::LoadModel("Ball", "../Engine/Engine/Renderer/Model/NewBall.gltf");
	ModelManager::LoadModel("Cube", "../Engine/Engine/Renderer/Model/Cube.gltf");
	ModelManager::LoadModel("Skybox", "../Engine/Engine/Renderer/Model/Skybox.gltf");
	ModelManager::LoadModel("Human", "../Engine/Engine/Renderer/Model/Human.gltf");
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

	int count = 25;

	for (int i = 0; i < count; i++)
	{
		for (int j = 0; j < count; j++)
		{
			if (clipNum > 9)
				clipNum = 0;

			womanTextureNum = rand() % 5;

			auto animationCreationCallback = [translationScalar, i, j, count](GraphicsObject* go)
				{
					TexturedAnimatedGraphicsObject* ago = static_cast<TexturedAnimatedGraphicsObject*>(go);
					if (go != nullptr)
					{
						ago->Translate(glm::vec3(i * translationScalar + -count*3, 0.0f, j * translationScalar + -count*3));
						ago->SetClip(8);
						ago->SetAnimationSpeed(1.0f);
					}

					if (i == j)
					{
						GraphicsObjectManager::WireFrame(go, ObjectTypes::GraphicsObjectType::AnimatedTextured);
					}

				};
			//std::this_thread::sleep_for(std::chrono::milliseconds(10));
			GraphicsObjectManager::CreateTexturedAnimatedGraphicsObject(ModelManager::GetModel("Woman"), TextureManager::GetTexture("Woman2"), animationCreationCallback);

		}
	}

	

	GraphicsObjectManager::CreateTexturedStaticGraphicsObject(ModelManager::GetModel("DefaultRectangleWithDepth"), TextureManager::GetTexture("Grid"),
	[](GraphicsObject* go) 
	{
		TexturedStaticGraphicsObject* tsgo = static_cast<TexturedStaticGraphicsObject*>(go);
		tsgo->Scale({ 100.0f, 100.0f, 0.0f});
		tsgo->Rotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));

		tPress = new std::function<void(int)>([tsgo](int keyCode)
		{
			float rotSpeed = 1.0f * TimeManager::DeltaTime();
			tsgo->Rotate(rotSpeed, glm::vec3(1.0f, 0.0f, 0.0f));
		});

		yPress = new std::function<void(int)>([tsgo](int keyCode)
		{
			float rotSpeed = 1.0f * TimeManager::DeltaTime();
			tsgo->Rotate(rotSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
		});

		uPress = new std::function<void(int)>([tsgo](int keyCode)
		{
			float rotSpeed = 1.0f * TimeManager::DeltaTime();
			tsgo->Rotate(rotSpeed, glm::vec3(0.0f, 0.0f, 1.0f));
		});

		InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_T, tPress);
		InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_Y, yPress);
		InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_U, uPress);
	});

	//GraphicsObjectManager::CreateTexturedStatic2DGraphicsObject(ModelManager::GetModel("DefaultRectangleWithDepth"), TextureManager::GetTexture("Woman"), [](GraphicsObject* go)
	//{
	//	TexturedStatic2DGraphicsObject* tsgo = static_cast<TexturedStatic2DGraphicsObject*>(go);
	//	tsgo->ScaleObject({ 300.0f, 300.0f });
	//	tsgo->TranslateObject({ 500.0f, 500.0f });
	//});

	SetInput();
}

std::vector<std::function<void(int)>**> keyList = { &wPress, &aPress, &dPress, &lctrPress, &qPress, &ePress };

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
		float frameTime = 0.0016f;
		if ((TimeManager::SecondsSinceStart() - frame) > frameTime)
		{	
			transformSystem->Operate();
			skyboxGameObject->Update();
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

	delete transformSystem;
	delete skyboxGameObject;
}

void SetInput()
{
	wPress = new std::function<void(int)>([](int keyCode)
	{
		Camera& cam = CameraManager::GetCamera("MainCamera");
		float speed = 4.0f * TimeManager::DeltaTime();
		cam.Translate(cam.GetForwardVector() * speed);
	});

	aPress = new std::function<void(int)>([](int keyCode)
	{
		Camera& cam = CameraManager::GetCamera("MainCamera");
		float speed = 4.0f * TimeManager::DeltaTime();
		cam.Translate(cam.GetRightVector() * -speed);
	});

	sPress = new std::function<void(int)>([](int keyCode)
	{
		Camera& cam = CameraManager::GetCamera("MainCamera");
		float speed = 4.0f * TimeManager::DeltaTime();
		cam.Translate(cam.GetForwardVector() * -speed);
	});

	dPress = new std::function<void(int)>([](int keyCode)
	{
		Camera& cam = CameraManager::GetCamera("MainCamera");
		float speed = 4.0f * TimeManager::DeltaTime();
		cam.Translate(cam.GetRightVector() * speed);
	});

	lctrPress = new std::function<void(int)>([](int keyCode)
	{
		Camera& cam = CameraManager::GetCamera("MainCamera");
		float speed = 4.0f * TimeManager::DeltaTime();
		cam.Translate(glm::vec3(0.0f, -speed, 0.0f));
	});

	spacePress = new std::function<void(int)>([](int keyCode)
	{
		Camera& cam = CameraManager::GetCamera("MainCamera");
		float speed = 4.0f * TimeManager::DeltaTime();
		cam.Translate(glm::vec3(0.0f, speed, 0.0f));
	});

	qPress = new std::function<void(int)>([](int keyCode)
	{
		Camera& cam = CameraManager::GetCamera("MainCamera");
		float rotSpeed = 2.0f * TimeManager::DeltaTime();
		cam.Rotate(cam.GetUpVector(), rotSpeed);
	});

	ePress = new std::function<void(int)>([](int keyCode)
	{
		Camera& cam = CameraManager::GetCamera("MainCamera");
		float rotSpeed = 2.0f * TimeManager::DeltaTime();
		cam.Rotate(cam.GetUpVector(), -rotSpeed);
	});
	

	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_L, lPress);
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_W, wPress);
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_A, aPress);
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_S, sPress);
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_D, dPress);
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_Q, qPress);
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_E, ePress);
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_LEFT_CTRL, lctrPress);
	InputManager::RegisterCallbackForKeyState(KEY_PRESSED, KEY_SPACE, spacePress);
}