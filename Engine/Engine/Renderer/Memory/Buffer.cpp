#include "Buffer.h"

#include "../Vulkan/Utils/VulkanUtils.h"
#include "../Renderer.h"
#include "../Vulkan/VulkanPhysicalDevice.h"
#include "../Commands/CommandManager.h"
#include "MemoryManager.h"

#include <stdexcept>

const VkBuffer& Buffer::operator()() const
{
    return buffer;
}

Buffer::Buffer(unsigned int sizeInBytes, VkBufferUsageFlags usage, VmaAllocationCreateFlagBits memProperties) :
    bufferCreateInfo(),
    buffer(VK_NULL_HANDLE),
    memoryAllocationCreateInfo(),
    memory(VK_NULL_HANDLE),
    usageFlags(usage),
    memoryPropertyFlags(memProperties),
    data(nullptr)
{
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext = nullptr;
    bufferCreateInfo.flags = 0;
    bufferCreateInfo.size = static_cast<uint64_t>(sizeInBytes);
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.queueFamilyIndexCount = 0;
    bufferCreateInfo.pQueueFamilyIndices = nullptr;
    bufferCreateInfo.usage = usageFlags;

    memoryAllocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
    memoryAllocationCreateInfo.flags = memoryPropertyFlags;

    VkDevice& device = Renderer::GetVulkanPhysicalDevice()->GetLogicalDevice();

    VkResult result = vmaCreateBuffer(MemoryManager::GetAllocator(), &bufferCreateInfo, &memoryAllocationCreateInfo, &buffer, &memory, nullptr);

    if (result != VK_SUCCESS)
    {
        Logger::Log(std::string("Failed to create a Vertex Buffer."), Logger::Category::Error);
        throw std::runtime_error("Failed to create a Vertex Buffer.");
    }

    if (result != VK_SUCCESS)
    {
        Logger::Log(std::string("Failed to bind buffer memory to buffer."), Logger::Category::Error);
        throw std::runtime_error("Failed to bind buffer memory to buffer.");
    }
}

Buffer::~Buffer()
{
    vmaDestroyBuffer(MemoryManager::GetAllocator(), buffer, memory);
}

unsigned int Buffer::FindMemoryType(unsigned int typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
    vkGetPhysicalDeviceMemoryProperties((*(*Renderer::GetVulkanPhysicalDevice())), &physicalDeviceMemoryProperties);

    for (unsigned int i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; i++)
    {
        if (typeFilter & (1 << i))
        {
            if ((physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }
    }

    Logger::Log(std::string("Failed to find a suitable memory type for a buffer."), Logger::Category::Error);

    throw std::runtime_error("Failed to find a suitable memory type for a buffer.");

    return 0;
}

void Buffer::Map(void* inData, unsigned int sizeInBytes)
{
    if ((memoryPropertyFlags & VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT))
    {
        VkDevice& device = Renderer::GetVulkanPhysicalDevice()->GetLogicalDevice();

        void* data;
        
        VkResult result = vmaMapMemory(MemoryManager::GetAllocator(), memory, &data);
        if (result != VK_SUCCESS)
        {
            Logger::Log(std::string("Failed to map buffer memory."), Logger::Category::Error);
            throw std::runtime_error("Failed to map buffer memory.");
            return;
        }

        memcpy(data, inData, static_cast<size_t>(bufferCreateInfo.size));
        
        vmaUnmapMemory(MemoryManager::GetAllocator(), memory);
    }
    else
    {
        Logger::Log(std::string("Calling Buffer::Map() on a buffer that cannot be mapped."), Logger::Category::Error);
        throw std::runtime_error("Calling Buffer::Map() on a buffer that cannot be mapped.");
        return;
    }
}

void Buffer::PersistentMap()
{
    if ((memoryPropertyFlags & VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT))
    {
        VkDevice& device = Renderer::GetVulkanPhysicalDevice()->GetLogicalDevice();

        VkResult result = vmaMapMemory(MemoryManager::GetAllocator(), memory, &data);
        if (result != VK_SUCCESS)
        {
            Logger::Log(std::string("Failed to map buffer memory."), Logger::Category::Error);
            throw std::runtime_error("Failed to map buffer memory.");
            return;
        }
    }
}

void Buffer::Unmap()
{
    vmaUnmapMemory(MemoryManager::GetAllocator(), memory);
}

void Buffer::SetData(void* inData)
{
    memcpy(data, inData, static_cast<size_t>(bufferCreateInfo.size));
}

void Buffer::CopyFrom(const Buffer& otherBuffer)
{
    int x = usageFlags & VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    if (!(usageFlags & VK_BUFFER_USAGE_TRANSFER_DST_BIT))
    {
        Logger::Log(std::string("Trying to copy to a buffer that is not a transfer destination buffer."));
        throw std::runtime_error("Trying to copy to a buffer that is not a transfer destination buffer.");
        return;
    }

    if (!(otherBuffer.usageFlags & VK_BUFFER_USAGE_TRANSFER_SRC_BIT))
    {
        Logger::Log(std::string("Invalid source buffer Buffer::CopyFrom()."));
        throw std::runtime_error("Invalid source buffer Buffer::CopyFrom().");
        return;
    }


    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    VkCommandBuffer& transferCommandBuffer = CommandManager::GetTransferCommandBuffer();

    VkResult result = vkBeginCommandBuffer(transferCommandBuffer, &beginInfo);
    VulkanUtils::CheckResult(result, true, true, "Failed to begin command buffer Buffer::CopyFrom().");


    VkBufferCopy bufferCopy = {};
    bufferCopy.size = otherBuffer.bufferCreateInfo.size;
    bufferCopy.dstOffset = 0;
    bufferCopy.srcOffset = 0;

    vkCmdCopyBuffer(transferCommandBuffer, otherBuffer.buffer, buffer, 1, &bufferCopy);
    result = vkEndCommandBuffer(transferCommandBuffer);
    VulkanUtils::CheckResult(result, true, true, "Failed to end command buffer Buffer::CopyFrom().");

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &transferCommandBuffer;

    VkQueue& graphicsQueue = Renderer::GetVulkanPhysicalDevice()->GetGraphicsQueue();
    result = vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    VulkanUtils::CheckResult(result, true, true, "Failed to submit transfer command buffer to the graphics queue Buffer::CopyFrom().");
    result = vkQueueWaitIdle(graphicsQueue);
    VulkanUtils::CheckResult(result, true, true, "QueueWaitIdle failed Buffer::CopyFrom().");
}

unsigned int Buffer::Size() const
{
    return static_cast<unsigned int>(bufferCreateInfo.size);
}
