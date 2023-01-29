#include "WindowManager.h"

#include <stdexcept>

#include <GLFW/glfw3.h>

#include "../../Utils/Logger.h"
#include "Window.h"

WindowManager* WindowManager::instance = nullptr;

void WindowManager::Initialize()
{
	if (!instance)
	{
		instance = new WindowManager();
		Logger::Log(std::string("Initialized WindowManager"), Logger::Category::Success);
		return;
	}

	Logger::Log(std::string("Calling WindowManager::Initialize when the WindowManager has already been initialized."), Logger::Category::Warning);
}

void WindowManager::Terminate()
{
	if (instance)
	{
		delete instance;
		Logger::Log(std::string("Terminated WindowManager"), Logger::Category::Success);
		return;
	}

	Logger::Log(std::string("Calling WindowManager::Terminate when the WindowManager has already been terminated."), Logger::Category::Warning);
}

Window& WindowManager::CreateWindow(uint32_t&& width, uint32_t&& height, std::string&& name)
{
	if (!instance)
	{
		Logger::Log(std::string("Calling WindowManager::CreateWindow before WindowManager::Initialize"), Logger::Category::Error);
		throw std::runtime_error("Calling WindowManager::CreateWindow before WindowManager::Initialize");
	}

	// Check if the window name is taken and if it is add a number to the end of the name.
	uint32_t windowNameDupNumber = 0;
	while (instance->windows.find((windowNameDupNumber == 0) ? name : name + std::to_string(windowNameDupNumber)) != instance->windows.end())
	{
		windowNameDupNumber++;
	}

	Window* newWindow = new Window(width, height, std::string((windowNameDupNumber == 0) ? name : name + std::to_string(windowNameDupNumber)));
	instance->windows[newWindow->GetName()] = newWindow;

	return *newWindow;
}

bool WindowManager::Update()
{
	if (!instance)
	{
		Logger::Log(std::string("Calling WindowManager::Update when the WindowManager has not been initialized."), Logger::Category::Error);
		throw std::runtime_error("Calling WindowManager::Update when the WindowManager has not been initialized.");
	}

	std::vector<std::string> windowsToRemove;
	windowsToRemove.reserve(instance->windows.size());

	// Update all the windows
	for (auto& window : instance->windows)
	{
		if (!window.second->Update())
		{
			windowsToRemove.push_back(window.first);
		}
	}

	// Remove windows that have closed
	for (const std::string& windowName : windowsToRemove)
	{
		delete instance->windows[windowName];
		instance->windows.erase(windowName);
	}

	if (instance->windows.size() > 0)
	{
		return true;
	}
	
	return false;
}

unsigned int WindowManager::WindowCount()
{
	return static_cast<unsigned int>(instance->windows.size());
}

Window* const WindowManager::GetWindow(std::string name)
{
	if (instance == nullptr)
	{
		Logger::LogAndThrow(std::string("Calling WindowManager::GetWindow() before WindowManager::Initialize()."));
		return nullptr;
	}

	if (instance->windows.find(name) == instance->windows.end())
	{
		Logger::Log(std::string("Could not find window ") + name, Logger::Category::Warning);
		return nullptr;
	}

	return instance->windows[name];
}

WindowManager::WindowManager() :
	windows(std::unordered_map<std::string, Window*>())
{
	if (glfwInit() != GLFW_TRUE)
	{
		Logger::Log(std::string("Could not initialize GLFW."), Logger::Category::Error);
		throw std::runtime_error("Could not initialize GLFW");
	}
}

WindowManager::~WindowManager()
{
	for (const std::pair<std::string, Window*>& window : windows)
	{
		delete window.second;
	}

	glfwTerminate();
}
