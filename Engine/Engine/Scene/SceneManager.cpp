#include "SceneManager.h"

#include "../Utils/Logger.h"
#include "Scene.h"

SceneManager* SceneManager::instance = nullptr;

void SceneManager::Initialize()
{
	if (instance == nullptr)
	{
		instance = new SceneManager();
		Logger::Log(std::string("Initialized SceneManager"), Logger::Category::Success);
	}
	else
	{
		Logger::Log(std::string("SceneManager::Initialize called before SceneManager::Terminate"), Logger::Category::Error);
	}

}

void SceneManager::Terminate()
{
	if (instance != nullptr)
	{
		delete instance;
		Logger::Log(std::string("Terminated SceneManager"), Logger::Category::Success);
	}
	else
	{
		Logger::Log(std::string("SceneManager::Terminate begin called before SceneManager::Initialize"), Logger::Category::Error);
	}
}

Scene* SceneManager::CreateScene(const std::string& name)
{
	if (instance != nullptr)
	{
		if (instance->scenes.find(name) == instance->scenes.end())
		{
			return instance->scenes[name] = new Scene();
		}
		else
		{
			Logger::Log(std::string("Could not create a scene with the name: ") + name + std::string(" a scene with that name already exsists."), Logger::Category::Warning);
		}
	}

	return nullptr;
}

SceneManager::SceneManager()
{

}

SceneManager::~SceneManager()
{
	for (auto& scene : scenes)
	{
		delete scene.second;
	}
}
