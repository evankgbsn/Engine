#ifndef SceneManager_H
#define SceneManager_H

#include <unordered_map>
#include <string>

class Scene;

class SceneManager
{
public:

	static void Initialize();

	static void Terminate();

	static Scene* CreateScene(const std::string& name);

protected:

private:

	SceneManager();

	~SceneManager();

	SceneManager(const SceneManager&) = delete;

	SceneManager& operator=(const SceneManager&) = delete;

	SceneManager(SceneManager&&) = delete;

	SceneManager& operator=(SceneManager&&) = delete;

	static SceneManager* instance;

	std::unordered_map<std::string, Scene*> scenes;
};

#endif //SceneManager_H