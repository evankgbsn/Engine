#include "Engine.h"

#include "Utils/Logger.h"
#include "Renderer/Renderer.h"
#include "Renderer/Windows//WindowManager.h"
#include "Renderer/Windows/Window.h"

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
	Window& mainWindow = WindowManager::CreateWindow(1920, 1080, "MainWindow");
	mainWindow.Initialize();
	while(Renderer::Update());
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

Engine::Engine(const std::string& gn, const Version& gv) :
	gameName(gn),
	gameVersion(gv),
	engineVersion(Version(1,0,0))
{
}

Engine::~Engine()
{
	Renderer::Terminate();
}