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

	static const char* invalidInstanceError = "Calling CommandManager::Initialize() before CommandManager::Terminate()";

	Logger::Log(std::string(invalidInstanceError), Logger::Category::Warning);
}

void CommandManager::Terminate()
{
	if (instance)
	{
		delete instance;
		return;
	}

	static const char* invalidInstanceError = "Calling CommandManager::Terminate() before CommandManager::Initialize()";

	Logger::Log(std::string(invalidInstanceError), Logger::Category::Warning);
}

VkCommandBuffer& CommandManager::GetCommandBuffer(const unsigned int id)
{
	static VkCommandBuffer invalidBuffer = VK_NULL_HANDLE;

	if (instance)
	{
		if (instance->buffers.size() > id)
		{
			return instance->buffers[id];
		}
		else
		{
			static const char* invalidIdErrorMsg = "Invalid id used in CommandManager::GetCommandBuffer";
			Logger::Log(std::string(invalidIdErrorMsg), Logger::Category::Error);
			throw std::runtime_error(invalidIdErrorMsg);
		}
	}

	const char* invalidInstanceErrorMsg = "Calling CommandManager::GetCommandBuffer() before CommandManager::Initialize()";

	Logger::Log(std::string(invalidInstanceErrorMsg), Logger::Category::Error);
	throw std::runtime_error(invalidInstanceErrorMsg);

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
		static const char* failedError = "Failed to create a command pool.";

		Logger::Log(std::string(failedError), Logger::Category::Error);
		throw std::runtime_error(failedError);
	}

	CreateCommandBuffers();
}

CommandManager::~CommandManager()
{
	vkDestroyCommandPool(Renderer::GetVulkanPhysicalDevice()->GetLogicalDevice(), commandPool, nullptr);
}

void CommandManager::CreateCommandBuffers()
{
	VulkanPhysicalDevice* physicalDevice = Renderer::GetVulkanPhysicalDevice();

	if (!physicalDevice)
	{
		static const char* physicalDeviceErrorMsg = "Failed to retrieve the vulkan physical device from renderer.";

		Logger::Log(std::string(physicalDeviceErrorMsg), Logger::Category::Error);
		throw std::runtime_error(physicalDeviceErrorMsg);
	}

	VkDevice device = physicalDevice->GetLogicalDevice();

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = Renderer::GetMaxFramesInFlight();

	buffers.resize(allocInfo.commandBufferCount);

	VkResult result = vkAllocateCommandBuffers(device, &allocInfo, buffers.data());

	if (result != VK_SUCCESS)
	{
		static const char* failedError = "Failed to allocate a command buffer.";

		Logger::Log(std::string(failedError), Logger::Category::Error);
		throw std::runtime_error(failedError);
	}
}
