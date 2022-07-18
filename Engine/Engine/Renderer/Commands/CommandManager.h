#ifndef COMMAND_MANAGER_H
#define COMMAND_MANAGER_H

#include <vulkan/vulkan.h>

class Window;

class CommandManager
{
public:

	static void Initialize();

	static void Terminate();

	static VkCommandBuffer& GetCommandBuffer();

private:

	CommandManager();

	~CommandManager();

	CommandManager(const CommandManager&) = delete;

	CommandManager& operator=(const CommandManager&) = delete;

	CommandManager(const CommandManager&&) = delete;

	CommandManager& operator=(const CommandManager&&) = delete;

	void CreateCommandBuffer();

	static CommandManager* instance;

	VkCommandPool commandPool = VK_NULL_HANDLE;

	VkCommandBuffer buffer = VK_NULL_HANDLE;
};

#endif // COMMAND_MANAGER_H