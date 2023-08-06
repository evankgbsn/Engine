#include "Engine.h"

#include "Scene/SceneManager.h"
#include "Scene/Scene.h"
#include "Entity/GameObject.h"
#include "System/TransformSystem.h"
#include "Component/TransformComponent.h"


#include <cstdio>

class HeapProfiling
{
public:
	~HeapProfiling()
	{

	};
};

static HeapProfiling heapProfiling;

void Game()
{
	Scene* main = SceneManager::CreateScene("Main");

	TransformSystem* transformSystem = new TransformSystem();

	main->AddSystem(std::string("Transform"), transformSystem);

	GameObject* gameObject = GameObject::Create();

	gameObject->AddComponent(Component::Type::TRANSFORM, transformSystem->CreateComponent());

	TransformComponent* transformComponent = gameObject->GetComponent<TransformComponent>(Component::Type::TRANSFORM);

	while (Engine::Operating());

	delete gameObject;
}

int main(int argc, const char** argv)
{
	const std::string gameName("FPS");
	const Engine::Version gameVersion(1, 0, 0);

	Engine::Initialize(gameName, gameVersion);
	Engine::SetGameThreadFunc(&Game);
	Engine::Start();
	Engine::Terminate();
	return 0;
}