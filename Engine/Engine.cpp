#include "Engine.h"
#include "Utils/Logger.h"

Engine* Engine::instance = nullptr;

void Engine::Initialize()
{
	if (!instance)
	{
		instance = new Engine();

		return;
	}
}