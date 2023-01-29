#include "CommandManager.h"

#include <stdexcept>

#include "../Renderer.h"
#include "../Vulkan/Utils/VulkanUtils.h"
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

VkCommandBuffer& CommandManager::GetRenderCommandBuffer()
{
	static VkCommandBuffer invalidBuffer = VK_NULL_HANDLE;

	if (instance)
	{
		return instance->renderBuffer;
	}

	VulkanUtils::CheckResult(static_cast<VkResult>(1), true, true, "Calling CommandManager::GetRenderCommandBuffer() before CommandManager::Initialize()", Logger::Category::Error);

	return invalidBuffer;
}

VkCommandBuffer& CommandManager::GetTransferCommandBuffer()
{
	static VkCommandBuffer invalidBuffer = VK_NULL_HANDLE;

	if (instance)
	{
		return instance->transferBuffer;
	}

	VulkanUtils::CheckResult(static_cast<VkResult>(1), true, true, "Calling CommandManager::GetRenderCommandBuffer() before CommandManager::Initialize()", Logger::Category::Error);

	return invalidBuffer;
}

CommandManager::CommandManager() :
	renderCommandPool(VK_NULL_HANDLE),
	renderBuffer(VK_NULL_HANDLE),
	transferCommandPool(VK_NULL_HANDLE),
	transferBuffer(VK_NULL_HANDLE)
{
	CreateCommandPools();
	CreateCommandBuffers();
}

CommandManager::~CommandManager()
{
	VkDevice& device = Renderer::GetVulkanPhysicalDevice()->GetLogicalDevice();

	vkDestroyCommandPool(device, renderCommandPool, nullptr);
	vkDestroyCommandPool(device, transferCommandPool, nullptr);
}

void CommandManager::CreateCommandPools()
{
	VulkanPhysicalDevice* const device = Renderer::GetVulkanPhysicalDevice();

	VkCommandPoolCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	createInfo.queueFamilyIndex = device->GetGraphicsQueueFamilyIndex();

	VkResult result = vkCreateCommandPool(device->GetLogicalDevice(), &createInfo, nullptr, &renderCommandPool);

	static const char* logMessage = "Failed to create a command pool.";
	VulkanUtils::CheckResult(result, true, true, logMessage, Logger::Category::Error);

	createInfo.flags = createInfo.flags | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

	result = vkCreateCommandPool(device->GetLogicalDevice(), &createInfo, nullptr, &transferCommandPool);

	VulkanUtils::CheckResult(result, true, true, logMessage, Logger::Category::Error);
}

void CommandManager::CreateCommandBuffers()
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = renderCommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	VkResult result = vkAllocateCommandBuffers(Renderer::GetVulkanPhysicalDevice()->GetLogicalDevice(), &allocInfo, &renderBuffer);

	static const char* logMessage = "Failed to allocate a command buffer.";
	VulkanUtils::CheckResult(result, true, true, logMessage, Logger::Category::Error);

	allocInfo.commandPool = transferCommandPool;

	result = vkAllocateCommandBuffers(Renderer::GetVulkanPhysicalDevice()->GetLogicalDevice(), &allocInfo, &transferBuffer);

	VulkanUtils::CheckResult(result, true, true, logMessage, Logger::Category::Error);
}
