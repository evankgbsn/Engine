#include "Engine.h"

#include "Scene/SceneManager.h"
#include "Scene/Scene.h"
#include "Entity/GameObject.h"
#include "System/TransformSystem.h"
#include "Component/TransformComponent.h"
#include "Renderer/Images/TextureManager.h"
#include "Renderer/Model/ModelManager.h"
#include "Renderer/GraphicsObjects/GraphicsObjectManager.h"
#include "Renderer/GraphicsObjects/TexturedAnimatedGraphicsObject.h"
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

static HeapProfiling heapProfiling;

void StressTest();
void LoadAssets();
void Game();

int main(int argc, const char** argv)
{
	const std::string gameName("FPS");
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
				GraphicsObject* ta = nullptr;
				GraphicsObjectManager::CreateLitTexturedStaticGraphicsObject(ModelManager::GetModel("Cube"), TextureManager::GetTexture("Woman3"), &ta);

				//while (ta == nullptr);
				//
				//TexturedAnimatedGraphicsObject* taGo = static_cast<TexturedAnimatedGraphicsObject*>(ta);
				//
				//if (taGo != nullptr)
				//{
				//	taGo->Translate(glm::vec3(i * translationScalar, j * translationScalar, k * translationScalar));
				//	taGo->SetClip(0);
				//	taGo->SetAnimationSpeed(1.f);
				//}
			}
		}
	}
	
	//GraphicsObjectManager::CreateGoochGraphicsObject(ModelManager::GetModel("Cruiser"), TextureManager::GetTexture("Woman"));
	//GraphicsObjectManager::CreateTexturedAnimatedGraphicsObject(ModelManager::GetModel("CruiserAnim"), TextureManager::GetTexture("Cruiser"));
}

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

	delete transformSystem;
	delete skyboxGameObject;

}