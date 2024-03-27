#include "InputManager.h"

#include "../Utils/Logger.h"

#include "../Renderer/Windows/WindowManager.h"
#include "../Renderer/Windows/Window.h"

#include <GLFW/glfw3.h>

InputManager* InputManager::instance = nullptr;

void InputManager::Initialize()
{
	if (instance == nullptr)
	{
		instance = new InputManager();
		Logger::Log(std::string("Initialized InputManager"), Logger::Category::Success);
	}
	else
	{
		Logger::Log(std::string("Calling InputManager::Initialize before InputManager::Terminate"), Logger::Category::Warning);
	}
}

void InputManager::Terminate()
{
	if (instance != nullptr)
	{
		delete instance;
		Logger::Log(std::string("Terminated InputManager"), Logger::Category::Success);
	}
	else
	{
		Logger::Log(std::string("Calling InputManager::Terminate before InputManager::Initialize"), Logger::Category::Warning);
	}
}

void InputManager::EnqueueInputCall(std::function<void()> inputCall)
{
	if (instance != nullptr)
	{
		std::lock_guard<std::mutex> guard(instance->inputQueueMutex);
		instance->inputQueue.push_back(inputCall);
	}
}

void InputManager::GetCursorPosition(std::function<void(const glm::vec2&)> callback)
{
	std::function<void()> getCursorPosition = [callback]()
	{
		const Window* const mainWindow = WindowManager::GetWindow("MainWindow");

		if (mainWindow != nullptr)
		{
			callback(mainWindow->GetCursorPosition());
		}
		else
		{
			Logger::Log(std::string("Failed to get cursor position. Could not get window reference. InputManager::GetCursorPosition"), Logger::Category::Warning);
			callback(glm::vec2(0.0f, 0.0f));
		}
	};
	
	EnqueueInputCall(getCursorPosition);
}

void InputManager::WhenCursorMoved(std::function<void(const glm::vec2& newCursorPosition)> callback)
{
	std::function<void()> getCursorPosition = [callback]()
	{
		const Window* const mainWindow = WindowManager::GetWindow("MainWindow");

		if (mainWindow != nullptr)
		{
			glm::vec2 newCursorPosition;
			bool moved = mainWindow->GetCursorMoved(newCursorPosition);
			
			if(moved) 
				callback(newCursorPosition);
		}
		else
		{
			Logger::Log(std::string("Failed to get cursor position. Could not get window reference. InputManager::GetCursorPosition"), Logger::Category::Warning);
			callback(glm::vec2(0.0f, 0.0f));
		}
	};

	EnqueueInputCall(getCursorPosition);
}

void InputManager::RegisterCallbackForKeyState(int state, int keyCode, std::function<void(int keyCode)>* callback)
{
	auto registerCallbackForKeyState = [keyCode, callback](std::unordered_map<int, std::list<std::function<void(int keyCode)>*>>& map, const std::string& successLog)
	{
		if (instance != nullptr)
		{
			map[keyCode].push_back(callback);

			char stringBuffer[5] = { '\0','\0','\0','\0','\0' };
			_itoa_s(keyCode, stringBuffer, 10);
			Logger::Log(successLog + std::string(&stringBuffer[0]), Logger::Category::Success);
		}
		else
		{
			Logger::Log(std::string("Calling InputManager::RegisterCallbackForKeyState before InputManager::Initialize"), Logger::Category::Warning);
		}
	};

	static const std::string pressedSuccessLog("Registered pressed callback for key code ");
	static const std::string pressSuccessLog("Registered press callback for key code ");
	static const std::string releaseSuccessLog("Registered release callback for key code ");
	static const std::string releasedSuccessLog("Registered released callback for key code ");

	std::function<void()> inputRegisterFunctionToQueue = [state, registerCallbackForKeyState]()
	{
		if (instance != nullptr)
		{
			switch (state)
			{
			case KEY_PRESS:
				registerCallbackForKeyState(instance->registeredKeyPressEvents, pressSuccessLog);
				break;
			case KEY_RELEASE:
				registerCallbackForKeyState(instance->registeredKeyReleaseEvents, releaseSuccessLog);
				break;
			case KEY_PRESSED:
				registerCallbackForKeyState(instance->registeredKeyPressedEvents, pressedSuccessLog);
				break;
			case KEY_RELEASED:
				registerCallbackForKeyState(instance->registeredKeyReleasedEvents, releasedSuccessLog);
				break;
			default:
				break;
			}
		}
	};

	EnqueueInputCall(inputRegisterFunctionToQueue);
}

void InputManager::DeregisterCallbackForKeyState(int state, int keyCode, std::function<void(int keyCode)>* callback)
{
	auto deregisterCallbackForKeyState = [keyCode, callback](std::unordered_map<int, std::list<std::function<void(int keyCode)>*>>& map, const std::string& successLog)
	{
		if (instance != nullptr)
		{
			if (map.find(keyCode) != map.end())
			{
				std::list<std::function<void(int keyCode)>*>& registeredCallbacksForKey = map[keyCode];
				std::list <std::function<void(int keyCode)>*>::iterator it = registeredCallbacksForKey.begin();

				for (it; it != registeredCallbacksForKey.end(); it)
				{
					const std::function<void(int keyCode)>* const registeredCallback = *it;

					if (registeredCallback == callback)
					{
						it = registeredCallbacksForKey.erase(it);
					}
				}
			}

			char stringBuffer[5] = { '\0','\0','\0','\0','\0' };
			_itoa_s(keyCode, stringBuffer, 10);
			Logger::Log(successLog + std::string(&stringBuffer[0]), Logger::Category::Success);
		}
		else
		{
			Logger::Log(std::string("Calling InputManager::DeregisterCallbackForKeyState before InputManager::Initialize"), Logger::Category::Warning);
		}
	};

	static const std::string pressedSuccessLog("Deregistered pressed callback for key code ");
	static const std::string pressSuccessLog("Deregistered press callback for key code ");
	static const std::string releaseSuccessLog("Deregistered release callback for key code ");

	std::function<void()> inputDeregisterFunctionToQueue = [state, deregisterCallbackForKeyState]()
	{
		if (instance != nullptr)
		{
			switch (state)
			{
			case KEY_PRESS:
				deregisterCallbackForKeyState(instance->registeredKeyPressEvents, pressSuccessLog);
				break;
			case KEY_RELEASE:
				deregisterCallbackForKeyState(instance->registeredKeyReleaseEvents, releaseSuccessLog);
				break;
			case KEY_PRESSED:
				deregisterCallbackForKeyState(instance->registeredKeyPressedEvents, releaseSuccessLog);
				break;
			case KEY_RELEASED:
				deregisterCallbackForKeyState(instance->registeredKeyReleasedEvents, releaseSuccessLog);
				break;
			default:
				break;
			}
		}
	};

	EnqueueInputCall(inputDeregisterFunctionToQueue); 
}

void InputManager::ProcessKeyEvents() const
{
	const Window* const mainWindow = WindowManager::GetWindow(std::string("MainWindow"));
	std::function<void(int state, const std::unordered_map<int, std::list<std::function<void(int keyCode)>*>>&)> processKeyEvents = [mainWindow](int state, const std::unordered_map<int, std::list<std::function<void(int keyCode)>*>>& map)
	{
		if (instance != nullptr)
		{
			for (const auto& keyCallbacks : map)
			{
				int keyState = mainWindow->GetKey(keyCallbacks.first);

				if (keyState == state)
				{
					for (const std::function<void(int keyCode)>* const func : keyCallbacks.second)
					{
						if (func != nullptr)
						{
							(*func)(keyCallbacks.first);
						}
					}
				}
			}
		}
	};

	if (instance != nullptr)
	{
		processKeyEvents(KEY_RELEASE, instance->registeredKeyReleaseEvents);
		processKeyEvents(KEY_PRESS, instance->registeredKeyPressEvents);
		processKeyEvents(KEY_PRESSED, instance->registeredKeyPressedEvents);
		processKeyEvents(KEY_RELEASED, instance->registeredKeyReleasedEvents);

		mainWindow->GetKey(KEY_0, true);
	}
}

void InputManager::Update()
{
	if (instance != nullptr)
	{
		if (!instance->inputQueue.empty())
		{
			for (std::function<void()> func : instance->inputQueue)
			{
				func();
			}

			std::list<std::function<void()>>::iterator it = instance->inputQueue.begin();
			for (it; it != instance->inputQueue.end();)
			{
				it = instance->inputQueue.erase(it);
			}
		}
		
		instance->ProcessKeyEvents();
	}
	else
	{
		Logger::Log(std::string("Calling InputManager::Update before InputManager::Initialize"), Logger::Category::Warning);
	}
}

InputManager::InputManager() :
	inputQueue(std::list<std::function<void()>>()),
	keysPressed(std::unordered_set<int>())
{

}

InputManager::~InputManager()
{
	instance = nullptr;
}
