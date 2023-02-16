#include "Image.h"

#include "../Vulkan/Utils/VulkanUtils.h"
#include "../Commands/CommandManager.h"
#include "../Renderer.h"
#include "../Vulkan/VulkanPhysicalDevice.h"
#include "StagingBuffer.h"

Image::Image(int width, int height, const StagingBuffer& stagingBuffer, unsigned int b) :
	image(VK_NULL_HANDLE),
	createInfo({}),
	binding(b)
{
	CreateImage(width, height, stagingBuffer);
	CreateImageView();
	CreateSampler();
}

Image::~Image()
{
	vkDestroySampler(Renderer::GetVulkanPhysicalDevice()->GetLogicalDevice(), sampler, nullptr);
	vkDestroyImageView(Renderer::GetVulkanPhysicalDevice()->GetLogicalDevice(), imageView, nullptr);
	vmaDestroyImage(MemoryManager::GetAllocator(), image, imageAllocation);
}

const VkImageView& Image::GetImageView() const
{
	return imageView;
}

const VkSampler& Image::GetSampler() const
{
	return sampler;
}

unsigned int Image::Binding() const
{
	return binding;
}

void Image::SetBinding(unsigned int newBinding)
{
	binding = newBinding;
}

void Image::CreateImage(int width, int height, const StagingBuffer& stagingBuffer)
{
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	createInfo.imageType = VK_IMAGE_TYPE_2D;
	createInfo.extent.width = static_cast<uint32_t>(width);
	createInfo.extent.height = static_cast<uint32_t>(height);
	createInfo.extent.depth = 1;
	createInfo.mipLevels = 1;
	createInfo.arrayLayers = 1;
	createInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
	createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	createInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	createInfo.flags = 0;

	imageAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
	imageAllocInfo.flags = 0;

	VkResult result = vmaCreateImage(MemoryManager::GetAllocator(), &createInfo, &imageAllocInfo, &image, &imageAllocation, nullptr);
	VulkanUtils::CheckResult(result, true, true, "Failed to create an image.");

	const VkCommandBuffer& transferBuffer = CommandManager::GetTransferCommandBuffer();

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	result = vkBeginCommandBuffer(transferBuffer, &beginInfo);
	VulkanUtils::CheckResult(result, true, true, "Failed to begin transfer command buffer Image().");

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.srcAccessMask = 0;
	barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

	vkCmdPipelineBarrier(transferBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = { static_cast<unsigned int>(width), static_cast<unsigned int>(height), 1U };

	vkCmdCopyBufferToImage(transferBuffer, stagingBuffer(), image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.srcAccessMask = 0;
	barrier.dstAccessMask = 0;
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

	vkCmdPipelineBarrier(transferBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

	result = vkEndCommandBuffer(transferBuffer);
	VulkanUtils::CheckResult(result, true, true, "Failed to end transfer command buffer Image().");

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &transferBuffer;

	const VkQueue& graphicsQueue = Renderer::GetVulkanPhysicalDevice()->GetGraphicsQueue();
	result = vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	VulkanUtils::CheckResult(result, true, true, "Failed to submit transfer command buffer Image().");
	vkQueueWaitIdle(graphicsQueue);
}

void Image::CreateImageView()
{
	VkImageViewCreateInfo imageViewCreateInfo{};
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.image = image;
	imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewCreateInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
	imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;

	VkResult result = vkCreateImageView(Renderer::GetVulkanPhysicalDevice()->GetLogicalDevice(), &imageViewCreateInfo, nullptr, &imageView);
	VulkanUtils::CheckResult(result, true, true, "Failed to create image view in Image().");
}

void Image::CreateSampler()
{
	VkSamplerCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	createInfo.magFilter = VK_FILTER_LINEAR;
	createInfo.minFilter = VK_FILTER_LINEAR;
	createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	createInfo.anisotropyEnable = VK_TRUE;
	createInfo.maxAnisotropy = Renderer::GetVulkanPhysicalDevice()->GetProperties().limits.maxSamplerAnisotropy;
	createInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_WHITE;
	createInfo.unnormalizedCoordinates = VK_FALSE;
	createInfo.compareEnable = VK_FALSE;
	createInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	createInfo.mipLodBias = 0.0f;
	createInfo.minLod = 0.0f;
	createInfo.maxLod = 0.0f;

	VkResult result = vkCreateSampler(Renderer::GetVulkanPhysicalDevice()->GetLogicalDevice(), &createInfo, nullptr, &sampler);
	VulkanUtils::CheckResult(result, true, true, "Failed to create sampler Image::CreateSampler().");
}
