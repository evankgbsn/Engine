#ifndef COMMAND_MANAGER_H
#define COMMAND_MANAGER_H

#include <vector>

#include <vulkan/vulkan.h>

class Window;

class CommandManager
{
public:

	static void Initialize();

	static void Terminate();

	static VkCommandBuffer& GetCommandBuffer(const unsigned int id);

private:

	CommandManager();

	~CommandManager();

	CommandManager(const CommandManager&) = delete;

	CommandManager& operator=(const CommandManager&) = delete;

	CommandManager(const CommandManager&&) = delete;

	CommandManager& operator=(const CommandManager&&) = delete;

	void CreateCommandBuffers();

	static CommandManager* instance;

	VkCommandPool commandPool = VK_NULL_HANDLE;

	std::vector<VkCommandBuffer> buffers;
};

#endif // COMMAND_MANAGER_H