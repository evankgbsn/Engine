#include "Editor.h"

#include "../../Utils/Logger.h"

Editor* Editor::instance = nullptr;

void Editor::Initialize()
{
	if (instance == nullptr)
	{
		instance = new Editor();
		Logger::Log(std::string("Initialzed Editor"), Logger::Category::Success);
	}
	else
	{
		Logger::Log(std::string("Calling Editor::Initialze() before Editor::Terminate()"), Logger::Category::Warning);
	}
}

void Editor::Terminate()
{
	if (instance != nullptr)
	{
		delete instance;
		Logger::Log(std::string("Terminated Editor"), Logger::Category::Success);
	}
	else
	{
		Logger::Log(std::string("Calling Editor::Terminate() before Logger::Initialize()"), Logger::Category::Warning);
	}
}

Editor::Editor()
{

}

Editor::~Editor()
{

}
