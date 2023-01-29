#include "MemoryManager.h"

#include "../../Utils/Logger.h"
#include "../Renderer.h"
#include "../Vulkan/VulkanPhysicalDevice.h"

MemoryManager* MemoryManager::instance = nullptr;

void MemoryManager::Initialize()
{
	if (instance == nullptr)
	{
		instance = new MemoryManager();
	}
}

void MemoryManager::Terminate()
{
	if (instance != nullptr)
	{
		delete instance;
	}
}

const VmaAllocator& MemoryManager::GetAllocator()
{
	if (instance == nullptr)
	{
		Logger::LogAndThrow("Calling MemoryManager::GetAllocator() bfore MemoryManager::Initialize().");
		static VmaAllocator defaultReturn;
		return defaultReturn;
	}

	return instance->allocator;
}

void MemoryManager::InitializeVMAAllocator()
{
	VmaAllocatorCreateInfo createInfo = {};
	createInfo.flags = 0;
	createInfo.physicalDevice = (*Renderer::GetVulkanPhysicalDevice())();
	createInfo.device = Renderer::GetVulkanPhysicalDevice()->GetLogicalDevice();
	createInfo.preferredLargeHeapBlockSize; // optional.
	createInfo.pAllocationCallbacks; // optional.
	createInfo.pDeviceMemoryCallbacks; // optional.
	createInfo.instance = Renderer::GetVulkanInstance();
	createInfo.vulkanApiVersion = VK_API_VERSION_1_3;

	vmaCreateAllocator(&createInfo, &allocator);
}

void MemoryManager::TerminateVMAAllocator()
{
	vmaDestroyAllocator(allocator);
}

MemoryManager::MemoryManager()
{
	InitializeVMAAllocator();
}

MemoryManager::~MemoryManager()
{
	TerminateVMAAllocator();
}
