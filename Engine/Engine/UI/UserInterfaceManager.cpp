#include "UserInterfaceManager.h"

#include "UserInterfaceItem.h"
#include "../Renderer/Model/ModelManager.h"
#include "../Renderer/Images/TextureManager.h"
#include "../Utils/Logger.h"
#include "../Renderer/Windows/Window.h"
#include "../Renderer/Windows/WindowManager.h"

UserInterfaceManager* UserInterfaceManager::instance = nullptr;

std::mutex UserInterfaceManager::instanceMutex = std::mutex();

void UserInterfaceManager::Initialize()
{
	std::lock_guard<std::mutex> guard(instanceMutex);

	if (instance == nullptr)
	{
		instance = new UserInterfaceManager();
		Logger::Log(std::string("Initialized UserInterfaceManager"), Logger::Category::Success);
	}
	else
	{
		Logger::Log(std::string("Calling UserInterfaceManager::Initialize before UserInterfaceManager::Terminate"), Logger::Category::Warning);
	}
}

void UserInterfaceManager::Terminate()
{
	if (instance != nullptr)
	{
		delete instance;
		Logger::Log(std::string("Termiated UserInterfaceManager"), Logger::Category::Success);
	}
	else
	{
		Logger::Log(std::string("Calling UserInterfaceManager::Terminate before UserInterfaceManager::Initialize"), Logger::Category::Warning);
	}
}

UserInterfaceItem* const UserInterfaceManager::CrateUserInterfaceItem(const std::string& name, Model* const model, Texture* const texture, const glm::vec2& position)
{
	std::lock_guard<std::mutex> guard(instanceMutex);

	if (instance != nullptr)
	{
		auto itemIterator = instance->userInterfaceItems.find(name);
		if (itemIterator == instance->userInterfaceItems.end())
		{
			Logger::Log(std::string("Created UserInterfaceItem: ") + name, Logger::Category::Success);
			return instance->userInterfaceItems[name] = new UserInterfaceItem(name, model, texture, position);
		}
		else
		{
			Logger::Log(std::string("Failed to create UserInterfaceItem ") + name + " a UserInterfaceItem with this name already exists.", Logger::Category::Success);
			return nullptr;
		}
	}
	else
	{
		return nullptr;
	}
}

void UserInterfaceManager::DestroyUserInterfaceItem(const std::string& name, bool subItems)
{
	std::lock_guard<std::mutex> guard(instanceMutex);

	if (instance != nullptr)
	{
		auto itemIterator = instance->userInterfaceItems.find(name);
		if (itemIterator->second != nullptr)
		{
			if (subItems)
			{
				const std::unordered_map<std::string, UserInterfaceItem*>& subItems = itemIterator->second->GetSubItems();

				std::list<std::string> keys;

				for (const auto& subItem : subItems)
				{
					keys.push_back(subItem.first);
				}

				for (const auto& key : keys)
				{
					auto subItemIterator = instance->userInterfaceItems.find(key);
					if (subItemIterator->second != nullptr)
					{
						delete subItemIterator->second;
						instance->userInterfaceItems.erase(subItemIterator);
					}
				}
			}

			delete itemIterator->second;
			instance->userInterfaceItems.erase(itemIterator);
		}
	}
}

UserInterfaceItem* const UserInterfaceManager::GetUserInterfaceItem(const std::string& name)
{
	std::lock_guard<std::mutex> guard(instanceMutex);

	if (instance != nullptr)
	{
		const auto& itemIterator = instance->userInterfaceItems.find(name);
		if (itemIterator != instance->userInterfaceItems.end())
		{
			return itemIterator->second;
		}
		else
		{
			Logger::Log(std::string("Failed to get UserInterfaceItem ") + name, Logger::Category::Warning);
			return nullptr;
		}
	}
	else
	{
		return nullptr;
	}
}

float UserInterfaceManager::GetWindowWidth()
{
	if (instance != nullptr)
	{
		return (float)instance->windowWidth;
	}

	return 0.0f;
}

float UserInterfaceManager::GetWindowHeight()
{
	if (instance != nullptr)
	{
		return (float)instance->windowHeight;
	}

	return 0.0f;
}

float UserInterfaceManager::GetPreviousWindowWidth()
{
	if (instance != nullptr)
	{
		return (float)instance->previousWindowWidth;
	}

	return 0.0f;
}

float UserInterfaceManager::GetPreviousWindowHeight()
{
	if (instance != nullptr)
	{
		return (float)instance->previousWindowHeight;
	}

	return 0.0f;
}

void UserInterfaceManager::OnWindowSizeUpdate(const Window* const window)
{
	if (instance != nullptr)
	{
		const Window* const window = WindowManager::GetWindow("MainWindow");

		if (window != nullptr)
		{
			instance->windowWidth = window->GetWidth();
			instance->windowHeight = window->GetHeight();
		}

		for (auto& userInterfaceItem : instance->userInterfaceItems)
		{
			userInterfaceItem.second->OnWindowSizeUpdate();
		}

		instance->previousWindowWidth = instance->windowWidth;
		instance->previousWindowHeight = instance->windowHeight;
	}
}

UserInterfaceManager::UserInterfaceManager() :
	userInterfaceItems(std::unordered_map<std::string, UserInterfaceItem*>()),
	windowWidth(0.0f),
	windowHeight(0.0f),
	previousWindowWidth(windowWidth),
	previousWindowHeight(windowHeight)
{
	const Window* const window = WindowManager::GetWindow("MainWindow");

	if (window != nullptr)
	{
		windowWidth = previousWindowWidth = window->GetWidth();
		windowHeight = previousWindowHeight = window->GetHeight();
	}
}

UserInterfaceManager::~UserInterfaceManager()
{
	std::lock_guard<std::mutex> guard(instanceMutex);

	for (const auto& userInterfaceItem : userInterfaceItems)
	{
		delete userInterfaceItem.second;
	}

	userInterfaceItems.clear();

	instance = nullptr;
}
