#include "Buffer.h"

#include <stdexcept>

#include "../../Utils/Logger.h"
#include "../Renderer.h"
#include "../Vulkan/VulkanPhysicalDevice.h"

VkBuffer& Buffer::operator*()
{
	return buffer;
}

Buffer::~Buffer()
{
	if (buffer != VK_NULL_HANDLE)
	{
		VulkanPhysicalDevice* physicalDevice = Renderer::GetVulkanPhysicalDevice();

		if (!physicalDevice)
		{
			static const char* physicalDeviceRefError = "Failed to get the vulkan physical device reference in Buffer::~Buffer().";
			Logger::Log(std::string(physicalDeviceRefError), Logger::Category::Error);
			return;
		}

		vkDestroyBuffer(physicalDevice->GetLogicalDevice(), buffer, nullptr);

		vkFreeMemory(physicalDevice->GetLogicalDevice(), memory, nullptr);
	}
}

void Buffer::SetData(void* data)
{
	VulkanPhysicalDevice* physicalDevice = Renderer::GetVulkanPhysicalDevice();

	if (!physicalDevice)
	{
		static const char* physicalDeviceRefError = "Failed to get the vulkan physical device reference in Buffer::SetData().";
		Logger::Log(std::string(physicalDeviceRefError), Logger::Category::Error);
		throw std::runtime_error(physicalDeviceRefError);
		return;
	}

	void* mappedMemory = nullptr;
	VkResult result = vkMapMemory(physicalDevice->GetLogicalDevice(), memory, 0, createInfo.size, 0, &mappedMemory);

	if (result != VK_SUCCESS)
	{
		static const char* mapFailureMsg = "Failed to map memory in Buffer::AllocateMemory().";
		Logger::Log(std::string(mapFailureMsg), Logger::Category::Error);
		throw std::runtime_error(mapFailureMsg);
		return;
	}

	memcpy(mappedMemory, data, createInfo.size);

	vkUnmapMemory(physicalDevice->GetLogicalDevice(), memory);
}

Buffer::Buffer(uint32_t size)
{
	createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	createInfo.size = size;
}

void Buffer::FindMemoryType()
{
	VulkanPhysicalDevice* physicalDevice = Renderer::GetVulkanPhysicalDevice();

	if (!physicalDevice)
	{
		static const char* physicalDeviceRefError = "Failed to get the vulkan physical device reference in Buffer::FindMemoryType().";
		Logger::Log(std::string(physicalDeviceRefError), Logger::Category::Error);
		throw std::runtime_error(physicalDeviceRefError);
		return;
	}

	const VkPhysicalDeviceMemoryProperties& memoryProperties = physicalDevice->GetMemoryProperties();

	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
	{
		if (memoryReqs.memoryTypeBits & (1 << i) && 
			(memoryProperties.memoryTypes[i].propertyFlags & (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
		)
		{
			memoryType = i;
			return;
		}
	}

	static const char* memoryTypeError = "Failed to find a suitable memory type for a buffer. Buffer::FindMemoryType().";
	Logger::Log(std::string(memoryTypeError), Logger::Category::Error);
	throw std::runtime_error(memoryTypeError);
}

void Buffer::AllocateMemory()
{
	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memoryReqs.size;
	allocInfo.memoryTypeIndex = memoryType;

	VulkanPhysicalDevice* physicalDevice = Renderer::GetVulkanPhysicalDevice();

	if (!physicalDevice)
	{
		static const char* physicalDeviceRefError = "Failed to get the vulkan physical device reference in Buffer::AllocateMemory().";
		Logger::Log(std::string(physicalDeviceRefError), Logger::Category::Error);
		throw std::runtime_error(physicalDeviceRefError);
		return;
	}

	VkResult result = vkAllocateMemory(physicalDevice->GetLogicalDevice(), &allocInfo, nullptr, &memory);

	if (result != VK_SUCCESS)
	{
		static const char* allocFailureMsg = "Failed to allocate device memory for a buffer in Buffer::AllocateMemory().";
		Logger::Log(std::string(allocFailureMsg), Logger::Category::Error);
		throw std::runtime_error(allocFailureMsg);
		return;
	}

	result = vkBindBufferMemory(physicalDevice->GetLogicalDevice(), buffer, memory, 0);

	if (result != VK_SUCCESS)
	{
		static const char* bindFailureMsg = "Failed to bind a buffer to memory in Buffer::AllocateMemory().";
		Logger::Log(std::string(bindFailureMsg), Logger::Category::Error);
		throw std::runtime_error(bindFailureMsg);
		return;
	}
}

void Buffer::CreateBuffer()
{
	VulkanPhysicalDevice* physicalDevice = Renderer::GetVulkanPhysicalDevice();

	if (!physicalDevice)
	{
		static const char* physicalDeviceRefError = "Failed to get the vulkan physical device reference in Buffer::CreateBuffer().";
		Logger::Log(std::string(physicalDeviceRefError), Logger::Category::Error);
		throw std::runtime_error(physicalDeviceRefError);
		return;
	}

	VkResult result = vkCreateBuffer(physicalDevice->GetLogicalDevice(), &createInfo, nullptr, &buffer);

	if (result != VK_SUCCESS)
	{
		static const char* createBufferError = "Failed to create a buffer. Buffer::CreateBuffer().";
		Logger::Log(std::string(createBufferError), Logger::Category::Error);
		throw std::runtime_error(createBufferError);
		return;
	}

	vkGetBufferMemoryRequirements(physicalDevice->GetLogicalDevice(), buffer, &memoryReqs);

	FindMemoryType();

	AllocateMemory();

	Logger::Log(std::string("Created buffer of size: ") + std::to_string(createInfo.size) + " bytes", Logger::Category::Success);
}
