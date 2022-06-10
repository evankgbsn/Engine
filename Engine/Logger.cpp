#include "Logger.h"

#include <iostream>

#ifdef _WIN32
#include "Windows.h"
#endif

void Logger::Log(std::string log, Category category)
{

	//TODO: Add support for color changes to log on other platforms.
#ifdef _WIN32

	// Set the color of the console text.
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	static const int originalConsoleTextColor = 15;
	static int consoleTextColor = 15;

	switch (category)
	{
	case Category::Error:
		consoleTextColor = 12;
		break;
	case Category::Warning:
		consoleTextColor = 14;
		break;
	case Category::Info:
		consoleTextColor = 15;
		break;
	case Category::Success:
		consoleTextColor = 10;
	default:
		log += std::string(" : Invalid log category used in Logger::Log() : ") + std::to_string(static_cast<int>(category));
		break;
	}

	SetConsoleTextAttribute(hConsole, consoleTextColor);

#endif
	
	std::cout << log << std::endl;

#ifdef _WIN32

	// Set the color of the console text back to what it was originally.
	SetConsoleTextAttribute(hConsole, originalConsoleTextColor);

#endif
}
