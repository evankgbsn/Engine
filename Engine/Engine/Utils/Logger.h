#ifndef LOGGER_H
#define LOGGER_H

#include <string>

class Logger
{

public:

	enum class Category
	{
		// Yellow
		Warning,
		// Red
		Error,
		// White (default)
		Info,
		// Green
		Success
	};

	// Log a string to the console.
	static void Log(std::string&& log, Category category = Category::Info, bool logToConsole = true, bool logToLogFile = true);

	// Clear the log.
	static void ClearLog(bool clearLogFileAlso = true);

	// Update log file path.
	static void SetLogFilePath(const std::string path);

	static std::string GetLogFilePath();

	// Log and then throw a runtime error with the same message.
	static void LogAndThrow(std::string&& log);

private:

	static std::string logFilePath;
};

#endif // LOGGER_H
