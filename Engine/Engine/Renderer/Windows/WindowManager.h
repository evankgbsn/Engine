#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <unordered_map>
#include <string>

class Window;

class WindowManager
{

public:

	static void Initialize();

	static void Terminate();

	static Window& CreateWindow(uint32_t&& width, uint32_t&& height, std::string&& name);

	static bool Update();

private:

	WindowManager();

	~WindowManager();

	WindowManager(const WindowManager&) = delete;

	WindowManager& operator=(const WindowManager&) = delete;

	WindowManager(const WindowManager&&) = delete;

	WindowManager& operator=(const WindowManager&&) = delete;

	static WindowManager* instance;

	std::unordered_map<std::string, Window*> windows;
};

#endif // WINDOW_MANAGER_H