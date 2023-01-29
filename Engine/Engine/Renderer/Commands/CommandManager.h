#ifndef COMMAND_MANAGER_H
#define COMMAND_MANAGER_H

#include <vulkan/vulkan.h>

class Window;

class CommandManager
{
public:

	static void Initialize();

	static void Terminate();

	static VkCommandBuffer& GetRenderCommandBuffer();

	static VkCommandBuffer& GetTransferCommandBuffer();

private:

	CommandManager();

	~CommandManager();

	CommandManager(const CommandManager&) = delete;

	CommandManager& operator=(const CommandManager&) = delete;

	CommandManager(const CommandManager&&) = delete;

	CommandManager& operator=(const CommandManager&&) = delete;

	void CreateCommandPools();

	void CreateCommandBuffers();

	static CommandManager* instance;

	VkCommandPool renderCommandPool;

	VkCommandBuffer renderBuffer;

	VkCommandPool transferCommandPool;

	VkCommandBuffer transferBuffer;
};

#endif // COMMAND_MANAGER_H