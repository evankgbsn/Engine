#include "Logger.h"

#include <iostream>
#include <fstream>

#ifdef _WIN32
#include "Windows.h"
#endif

std::string Logger::logFilePath = std::string("Log.txt");

void Logger::Log(std::string&& log, Category category, bool logToConsole, bool logToLogFile)
{
	//TODO: Add support for color changes to log on other platforms.
#ifdef _WIN32

	// Set the color of the console text.
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	static const int originalConsoleTextColor = 15 /* White */;
	static int consoleTextColor = originalConsoleTextColor;

	static std::string categoryText("");
	categoryText.clear();

	switch (category)
	{
	case Category::Error:
		categoryText.append("Error: ");
		consoleTextColor = 12;
		break;
	case Category::Warning:
		categoryText.append("Warning: ");
		consoleTextColor = 14;
		break;
	case Category::Info:
		categoryText.append("Info: ");
		consoleTextColor = 15;
		break;
	case Category::Success:
		categoryText.append("Success: ");
		consoleTextColor = 10;
		break;
	default:
		log += std::string(" : Invalid log category used in Logger::Log() : ") + std::to_string(static_cast<int>(category));
		break;
	}

	SetConsoleTextAttribute(hConsole, consoleTextColor);

#endif

	// Log to the log file.
	if (logToLogFile)
	{
		std::ofstream ofstream(logFilePath.c_str(), std::ios_base::out | std::ios_base::app);
		ofstream << categoryText.append(log) << std::endl;
	}

	// Log to the console.
	if (logToConsole)
	{
		std::cout << categoryText << std::endl;
	}

#ifdef _WIN32

	// Set the color of the console text back to what it was originally.
	SetConsoleTextAttribute(hConsole, originalConsoleTextColor);

#endif
}

void Logger::ClearLog(bool clearLogFileAlso)
{

#ifdef _WIN32
	system("cls");
#endif // _WIN32

	if (clearLogFileAlso)
	{
		std::ofstream ofs;
		ofs.open(logFilePath, std::ofstream::out | std::ofstream::trunc);
		ofs.close();
	}
}

void Logger::SetLogFilePath(const std::string path)
{
	logFilePath = path;
}

std::string Logger::GetLogFilePath()
{
	return logFilePath;
}
