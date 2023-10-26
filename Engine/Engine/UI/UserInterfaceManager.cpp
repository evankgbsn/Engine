#include "UserInterfaceManager.h"

#include "../Utils/Logger.h"

UserInterfaceManager* UserInterfaceManager::instance = nullptr;

void UserInterfaceManager::Initialize()
{
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

UserInterfaceManager::UserInterfaceManager()
{

}

UserInterfaceManager::~UserInterfaceManager()
{
}
