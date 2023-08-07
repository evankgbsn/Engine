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

	ModelManager::LoadModel("Woman", "../Engine/Engine/Renderer/Model/Woman.gltf");
	ModelManager::LoadModel("Cruiser", "../Engine/Engine/Renderer/Model/Cruiser.gltf");
	ModelManager::LoadModel("CruiserAnim", "../Engine/Engine/Renderer/Model/CruiserAnim.gltf");
	ModelManager::LoadModel("Ball", "../Engine/Engine/Renderer/Model/NewBall.gltf");
	ModelManager::LoadModel("Human", "../Engine/Engine/Renderer/Model/Human.gltf");
	ModelManager::LoadModel("Cube", "../Engine/Engine/Renderer/Model/Cube.gltf");

	//StressTest();
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
				GraphicsObjectManager::CreateTexturedAnimatedGraphicsObject(ModelManager::GetModel("Cube"), TextureManager::GetTexture("VikingRoom"), &ta);

				while (ta == nullptr);

				TexturedAnimatedGraphicsObject* taGo = static_cast<TexturedAnimatedGraphicsObject*>(ta);

				taGo->Translate(glm::vec3(i * translationScalar, j * translationScalar, k * translationScalar));
				taGo->SetClip(0);
				taGo->SetAnimationSpeed(2.f);
			}
		}
	}
	
	//GraphicsObjectManager::CreateGoochGraphicsObject(ModelManager::GetModel("Cruiser"), TextureManager::GetTexture("Woman"));
	//GraphicsObjectManager::CreateTexturedAnimatedGraphicsObject(ModelManager::GetModel("CruiserAnim"), TextureManager::GetTexture("Cruiser"));
}

void Game()
{
	StressTest();

	//float translation = 3.0f;
	//for (unsigned int i = 0; i < 10; i++)
	//{
	//	ta->Translate(glm::vec3(i * translation, 0.0f, 0.0f));
	//}
	
	//TODO: Engine thread GraphicsObject Creation Commands.
	GraphicsObject* ta0 = nullptr;
	GraphicsObjectManager::CreateTexturedAnimatedGraphicsObject(ModelManager::GetModel("Cube"), TextureManager::GetTexture("VikingRoom"), &ta0);
	GraphicsObject* ta1 = nullptr;
	GraphicsObjectManager::CreateTexturedAnimatedGraphicsObject(ModelManager::GetModel("Cube"), TextureManager::GetTexture("VikingRoom"), &ta1);

	//Scene* main = SceneManager::CreateScene("Main");
	//
	//TransformSystem* transformSystem = new TransformSystem();
	//
	//main->AddSystem(std::string("Transform"), transformSystem);
	//
	//GameObject* gameObject = GameObject::Create();
	//
	//gameObject->AddComponent(Component::Type::TRANSFORM, transformSystem->CreateComponent());
	//
	//TransformComponent* transformComponent = gameObject->GetComponent<TransformComponent>(Component::Type::TRANSFORM);
	//
	while (Engine::Operating());

	//delete gameObject;
}