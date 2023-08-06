#include "Engine.h"

#include <cstdio>

class HeapProfiling
{
public:
	~HeapProfiling()
	{

	};
};

static HeapProfiling heapProfiling;

void Game()
{
	return;
}

int main(int argc, const char** argv)
{
	const std::string gameName("FPS");
	const Engine::Version gameVersion(1, 0, 0);

	Engine::Initialize(gameName, gameVersion);
	Engine::SetGameThreadFunc(&Game);
	Engine::Start();
	Engine::Terminate();
	return 0;
}