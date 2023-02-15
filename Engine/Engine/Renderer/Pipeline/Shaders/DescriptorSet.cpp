#include "DescriptorSet.h"

#include "DescriptorSetLayout.h"
#include "../../Vulkan/Utils/VulkanUtils.h"
#include "../../Renderer.h"
#include "../../Vulkan/VulkanPhysicalDevice.h"
#include "../../Memory/UniformBuffer.h"
#include "../../Memory/Image.h"
#include "../../GraphicsObjects/GraphicsObject.h"
#include "ShaderPipelineStage.h"
#include "Shader.h"

#include <vector>

DescriptorSet::DescriptorSet(const VkDescriptorPool& descriptorPool, const ShaderPipelineStage& shader, GraphicsObject* const graphicsObject) :
	allocInfo({}),
	descriptorSet(VK_NULL_HANDLE),
	pool(descriptorPool)
{
	VkDevice& device = Renderer::GetVulkanPhysicalDevice()->GetLogicalDevice();

	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &shader.GetDescriptorSetLayout()();

	VkResult result = vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet);
	VulkanUtils::CheckResult(result, true, true, "Failed to create a descriptor set.");

	std::vector<VkWriteDescriptorSet> writes;
	for (const VkDescriptorSetLayoutBinding& binding : shader.GetDescriptorSetLayout().GetLayoutBindings())
	{
		const UniformBuffer* uniformBuffer = nullptr;
		const Image* image = nullptr;
		switch (binding.descriptorType)
		{
		case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
			uniformBuffer = graphicsObject->GetUniformBuffer(binding.binding);

			if (uniformBuffer != nullptr)
			{
				VkDescriptorBufferInfo bufferInfo = {};
				bufferInfo.buffer = (*uniformBuffer)();
				bufferInfo.offset = 0;
				bufferInfo.range = static_cast<uint64_t>(uniformBuffer->Size());

				VkWriteDescriptorSet bufferWrite = {};
				bufferWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				bufferWrite.dstSet = descriptorSet;
				bufferWrite.dstBinding = uniformBuffer->Binding();
				bufferWrite.dstArrayElement = 0;
				bufferWrite.descriptorCount = 1;
				bufferWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				bufferWrite.pBufferInfo = &bufferInfo;

				writes.push_back(bufferWrite);
			}
			break;
		case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
			image = graphicsObject->GetImage(binding.binding);

			if (image != nullptr)
			{
				VkDescriptorImageInfo imageInfo = {};
				imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfo.imageView = image->GetImageView();
				imageInfo.sampler = image->GetSampler();

				VkWriteDescriptorSet imageWrite = {};
				imageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				imageWrite.dstSet = descriptorSet;
				imageWrite.dstBinding = image->Binding();
				imageWrite.dstArrayElement = 0;
				imageWrite.descriptorCount = 1;
				imageWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				imageWrite.pImageInfo = &imageInfo;
				writes.push_back(imageWrite);
			}
			break;
		default:
			break;
		}
	}

	vkUpdateDescriptorSets(device, static_cast<unsigned int>(writes.size()), writes.data(), 0, nullptr);
}

DescriptorSet::~DescriptorSet()
{

}

const VkDescriptorSet& DescriptorSet::operator()() const
{
	return descriptorSet;
}