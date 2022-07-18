#include "CommandManager.h"

#include <stdexcept>

#include "../Renderer.h"
#include "../../Utils/Logger.h"
#include "../Vulkan/VulkanPhysicalDevice.h"

CommandManager* CommandManager::instance = nullptr;

void CommandManager::Initialize()
{
	if (!instance)
	{
		instance = new CommandManager();
		return;
	}

	Logger::Log(std::string("Calling CommandManager::Initialize() before CommandManager::Terminate()"), Logger::Category::Warning);
}

void CommandManager::Terminate()
{
	if (instance)
	{
		delete instance;
		return;
	}

	Logger::Log(std::string("Calling CommandManager::Terminate() before CommandManager::Initialize()"), Logger::Category::Warning);
}

VkCommandBuffer& CommandManager::GetCommandBuffer()
{
	static VkCommandBuffer invalidBuffer = VK_NULL_HANDLE;

	if (instance)
	{
		return instance->buffer;
	}

	Logger::Log(std::string("Calling CommandManager::GetCommandBuffer() before CommandManager::Initialize()"), Logger::Category::Error);
	throw std::runtime_error("Calling CommandManager::GetCommandBuffer() before CommandManager::Initialize()");

	return invalidBuffer;
}

CommandManager::CommandManager()
{
	VulkanPhysicalDevice* const device = Renderer::GetVulkanPhysicalDevice();

	VkCommandPoolCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	createInfo.queueFamilyIndex = device->GetGraphicsQueueFamilyIndex();

	VkResult result = vkCreateCommandPool(device->GetLogicalDevice(), &createInfo, nullptr, &commandPool);

	if (result != VK_SUCCESS)
	{
		Logger::Log(std::string("Failed to create a command pool."), Logger::Category::Error);
		throw std::runtime_error("Failed to create a command pool.");
	}

	CreateCommandBuffer();
}

CommandManager::~CommandManager()
{
	vkDestroyCommandPool(Renderer::GetVulkanPhysicalDevice()->GetLogicalDevice(), commandPool, nullptr);
}

void CommandManager::CreateCommandBuffer()
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	VkResult result = vkAllocateCommandBuffers(Renderer::GetVulkanPhysicalDevice()->GetLogicalDevice(), &allocInfo, &buffer);

	if (result != VK_SUCCESS)
	{
		Logger::Log(std::string("Failed to allocate a command buffer."), Logger::Category::Error);
		throw std::runtime_error("Failed to allocate a command buffer.");
	}
}
