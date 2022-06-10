#include "Engine.h"

#include <cstdio>

int main(int argc, const char** argv)
{
	const std::string gameName("FPS");
	const Engine::Version gameVersion(1, 0, 0);

	Engine::Initialize(gameName, gameVersion);
	Engine::Start();
	Engine::Terminate();
	return 0;
}