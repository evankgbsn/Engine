#ifndef USERINTERFACEMANAGER_H
#define USERINTERFACEMANAGER_H

#include <string>
#include <unordered_map>
#include <mutex>

#include <glm/glm.hpp>

class Window;
class Model;
class Texture;
class UserInterfaceItem;

class UserInterfaceManager
{
public:

	static void Initialize();

	static void Terminate();

	static UserInterfaceItem* const CrateUserInterfaceItem(const std::string& name, Model* const model, Texture* const texture, float zOrder = 10.0f, const glm::vec2& position = glm::vec2(0.0f, 0.0f), const glm::vec2& scale = glm::vec2(1.0f, 1.0f));

	static void DestroyUserInterfaceItem(const std::string& name, bool subItems);

	static UserInterfaceItem* const GetUserInterfaceItem(const std::string& name);

	static float GetWindowWidth();

	static float GetWindowHeight();

	static float GetPreviousWindowWidth();

	static float GetPreviousWindowHeight();

private:
	
	friend class Window;

	static void OnWindowSizeUpdate(const Window* const window);

	UserInterfaceManager();

	~UserInterfaceManager();

	UserInterfaceManager(const UserInterfaceManager&) = delete;

	UserInterfaceManager& operator=(const UserInterfaceManager&) = delete;

	UserInterfaceManager(UserInterfaceManager&&) = delete;

	UserInterfaceManager& operator=(UserInterfaceManager&&) = delete;

	static UserInterfaceManager* instance;

	std::unordered_map<std::string, UserInterfaceItem*> userInterfaceItems;

	static std::mutex instanceMutex;

	float windowWidth;
	
	float windowHeight;

	float previousWindowWidth;

	float previousWindowHeight;
};

#endif // USERINTERFACEMANAGER_H