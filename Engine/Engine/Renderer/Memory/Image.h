#ifndef IMAGE_H
#define IMAGE_H

#include "MemoryManager.h"

#include <vulkan/vulkan.h>

class StagingBuffer;

class Image
{

public:

	Image() = delete;

	Image(int width, int height, const StagingBuffer& stagingBuffer, unsigned int binding);

	~Image();

	Image(const Image&) = delete;

	Image& operator=(const Image&) = delete;

	Image(Image&&) = delete;

	Image& operator=(Image&&) = delete;

	const VkDescriptorSetLayoutBinding& GetLayoutBinding() const;

	const VkImageView& GetImageView() const;

	const VkSampler& GetSampler() const;

	unsigned int Binding() const;

private:

	void CreateImage(int width, int height, const StagingBuffer& stagingBuffer);

	void CreateImageView();

	void CreateSampler();

	VkImage image;

	VkImageView imageView;

	VkSampler sampler;

	VkImageCreateInfo createInfo;

	VmaAllocation imageAllocation;

	VmaAllocationCreateInfo imageAllocInfo;

	VkDescriptorSetLayoutBinding layoutBinding;

};


#endif // IMAGE_H