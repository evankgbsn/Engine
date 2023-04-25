#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

class DirectionalLight;

class LightManager
{
public:

	static void Initialize();

	static void Terminate();

	// Creates a new directional light with the name provided. If a dircetional light already exists with that name it will return the existing directional light.
	static DirectionalLight* CreateDirectionalLight(const std::string& name, const glm::vec3& direction = glm::vec3(-0.5f, -0.5f, -0.5f), const glm::vec3& color = glm::vec3(1.0f, 1.0f, 1.0f));

	static DirectionalLight* GetDirectionalLight(const std::string& name);

private:

	LightManager();

	~LightManager();

	LightManager(const LightManager&) = delete;

	LightManager& operator=(const LightManager&) = delete;

	LightManager(LightManager&&) = delete;

	LightManager& operator=(LightManager&&) = delete;

private:

	static LightManager* instance;

	std::unordered_map<std::string, DirectionalLight*> directionalLights;
};

#endif //LIGHTMANAGER_H