#ifndef USERINTERFACEMANAGER_H
#define USERINTERFACEMANAGER_H

#include <string>
#include <unordered_map>
#include <mutex>

#include <glm/glm.hpp>

class Model;
class Texture;
class UserInterfaceItem;

class UserInterfaceManager
{
public:

	static void Initialize();

	static void Terminate();

	static UserInterfaceItem* const CrateUserInterfaceItem(const std::string& name, Model* const model, Texture* const texture, const glm::vec2& position);

	static void DestroyUserInterfaceItem(const std::string& name, bool subItems);

	static UserInterfaceItem* const GetUserInterfaceItem(const std::string& name);

private:
	
	UserInterfaceManager();

	~UserInterfaceManager();

	UserInterfaceManager(const UserInterfaceManager&) = delete;

	UserInterfaceManager& operator=(const UserInterfaceManager&) = delete;

	UserInterfaceManager(UserInterfaceManager&&) = delete;

	UserInterfaceManager& operator=(UserInterfaceManager&&) = delete;

	static UserInterfaceManager* instance;

	std::unordered_map<std::string, UserInterfaceItem*> userInterfaceItems;

	static std::mutex instanceMutex;

};

#endif // USERINTERFACEMANAGER_H