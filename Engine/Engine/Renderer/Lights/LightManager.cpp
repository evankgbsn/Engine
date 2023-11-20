#include "LightManager.h"

#include "../../Utils/Logger.h"
#include "DirectionalLight.h"

LightManager* LightManager::instance = nullptr;

std::mutex LightManager::instanceMutex = std::mutex();

void LightManager::Initialize()
{
	std::lock_guard<std::mutex> guard(instanceMutex);

	if (instance == nullptr)
	{
		instance = new LightManager();
		Logger::Log(std::string("Initialized LightManager"), Logger::Category::Success);
	}
	else
	{
		Logger::Log(std::string("Calling LightManager::Initialize() before LightManager::Terminate()."), Logger::Category::Warning);
	}
}

void LightManager::Terminate()
{
	std::lock_guard<std::mutex> guard(instanceMutex);

	if (instance != nullptr)
	{
		delete instance;
	}
	else
	{
		Logger::Log(std::string("Calling LightManager::Terminate() before LightManager::Initialize()"), Logger::Category::Warning);
	}
}

DirectionalLight* LightManager::CreateDirectionalLight(const std::string& name, const glm::vec3& direction, const glm::vec3& color)
{
	std::lock_guard<std::mutex> guard(instanceMutex);

	if (instance == nullptr)
	{
		Logger::Log(std::string("Calling LightManager::CreateDirectionalLight() before LightManager::Initialize()"), Logger::Category::Warning);
		return nullptr;
	}

	std::unordered_map<std::string, DirectionalLight*>::iterator existingLight = instance->directionalLights.find(name);
	if (existingLight != instance->directionalLights.end())
	{
		Logger::Log(std::string("DirectionalLight already exists with name: ") + name, Logger::Category::Warning);
		return existingLight->second;
	}

	return instance->directionalLights[name] = new DirectionalLight(direction, color);;
}

DirectionalLight* LightManager::GetDirectionalLight(const std::string& name)
{
	std::lock_guard<std::mutex> guard(instanceMutex);

	if (instance == nullptr)
	{
		Logger::Log(std::string("Calling LightManager::GetDirectionalLight() before LightManager::Initialize()"), Logger::Category::Warning);
		return nullptr;
	}

	std::unordered_map<std::string, DirectionalLight*>::iterator directionalLight = instance->directionalLights.find(name);
	if (directionalLight == instance->directionalLights.end())
	{
		Logger::Log(std::string("DirectionalLight does not exist with name: ") + name, Logger::Category::Warning);
		return nullptr;
	}

	return directionalLight->second;
}

LightManager::LightManager() :
	directionalLights(std::unordered_map<std::string, DirectionalLight*>())
{

}

LightManager::~LightManager()
{
	for (auto& directionalLight : directionalLights)
	{
		delete directionalLight.second;
	}

	instance = nullptr;
}
