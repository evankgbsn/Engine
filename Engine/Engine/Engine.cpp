#include "Engine.h"

#include "Utils/Logger.h"
#include "Renderer/Renderer.h"
#include "Renderer/Windows//WindowManager.h"
#include "Renderer/Windows/Window.h"
#include "Time/TimeManager.h"
#include "Scene/SceneManager.h"

Engine* Engine::instance = nullptr;

void Engine::Initialize(const std::string& gameName, const Version& gameVersion)
{
	if (!instance)
	{
		Logger::ClearLog();
		instance = new Engine(gameName, gameVersion);
		Renderer::Initialize();
		Logger::Log(std::string("Initialized Engine"), Logger::Category::Success);
		Logger::Log(std::string("Engine Version: ") + instance->engineVersion.ToString());
		Logger::Log(std::string("Game Version: ") + instance->gameVersion.ToString());
		return;
	}

	Logger::Log(std::string("Called Engine::Initialize when the engine has already been initialized"), Logger::Category::Warning);
}

void Engine::Terminate()
{
	if (instance)
	{
		delete instance;
		Logger::Log(std::string("Terminated Engine"), Logger::Category::Success);
		return;
	}

	Logger::Log(std::string("Called Engine::Terminate when the engine has already been terminated"), Logger::Category::Warning);
}

void Engine::Start()
{
	if (instance != nullptr)
	{
		Window& mainWindow = WindowManager::CreateWindow(1920, 1080, "MainWindow");
		mainWindow.Initialize();

		instance->LoadAssets();
		instance->SpawnAndDetachGameThread();

		while (instance->shouldUpdate)
		{
			TimeManager::RecordUpdateTime();
			instance->shouldUpdate = Renderer::Update();
		};
	}
}

bool Engine::Operating()
{
	if (instance != nullptr)
	{
		return instance->shouldUpdate;
	}

	return false;
}

const std::string& Engine::GetGameName()
{
	if (!instance)
	{
		Logger::Log(std::string("Calling Engine::GetGameName before Engine::Initialize()"), Logger::Category::Error);
		static const std::string invalidGameName("Invalid Game Name");
		return invalidGameName;
	}

	return instance->gameName;
}

const Engine::Version& Engine::GetGameVersion()
{
	if (!instance)
	{
		Logger::Log(std::string("Calling Engine::GetGameVersion before Engine::Initialize()"), Logger::Category::Error);
		static const Version invalidGameVersion(0,0,0);
		return invalidGameVersion;
	}

	return instance->gameVersion;
}

void Engine::SetGameThreadFunc(void(*newGameThreadFunc)())
{
	if (instance != nullptr)
	{
		if (instance->userGameThread == nullptr)
		{
			instance->userGameThreadFunc = newGameThreadFunc;
			Logger::Log(std::string("Engine::SetGameThreadFunc success"), Logger::Category::Success);
		}
	}
	else
	{
		Logger::Log(std::string("Calling Engine::SetAndSpawnGameThread before Engine::Initialize"), Logger::Category::Error);
	}
}

void Engine::SetLoadAssetsFunc(void(*newLoadAssetsFunc)())
{
	if (instance != nullptr)
	{
		instance->loadAssetsFunc = newLoadAssetsFunc;
	}
}

Engine::Engine(const std::string& gn, const Version& gv) :
	gameName(gn),
	gameVersion(gv),
	engineVersion(Version(1,0,0)),
	spawnedGameThreads(std::unordered_map<void(*)(), std::thread*>())
{
	TimeManager::Initialize();
	SceneManager::Initialize();
}

Engine::~Engine()
{
	for (std::pair<void(* const)(), std::thread*>& thread : spawnedGameThreads)
	{
		if(thread.second->joinable())
			thread.second->join();
		delete thread.second;
	}

	SceneManager::Terminate();
	Renderer::Terminate();
	TimeManager::Terminate();
}

void Engine::SpawnAndDetachGameThread()
{
	if (userGameThreadFunc != nullptr)
	{
		userGameThread = new std::thread(userGameThreadFunc);
		userGameThread->detach();
		spawnedGameThreads.insert(std::make_pair(userGameThreadFunc, userGameThread));
	}
}

void Engine::LoadAssets()
{
	loadAssetsFunc();
}
