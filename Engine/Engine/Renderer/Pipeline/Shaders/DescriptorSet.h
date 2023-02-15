#ifndef DESCRIPTORSET_H
#define DESCRIPTORSET_H

#include <vulkan/vulkan.h>

class UniformBuffer;
class DescriptorSetLayout;
class Image;
class ShaderPipelineStage;
class GraphicsObject;

class DescriptorSet
{

public:

	DescriptorSet() = delete;

	DescriptorSet(const VkDescriptorPool& descriptorPool, const ShaderPipelineStage& shader, GraphicsObject* const graphicsObject);

	~DescriptorSet();

	DescriptorSet(const DescriptorSet&) = delete;

	DescriptorSet& operator=(const DescriptorSet&) = delete;

	DescriptorSet(DescriptorSet&&) = delete;

	DescriptorSet& operator=(DescriptorSet&&) = delete;

	const VkDescriptorSet& operator()() const;

private:

	VkDescriptorSetAllocateInfo allocInfo;

	VkDescriptorSet descriptorSet;

	const VkDescriptorPool& pool;
};


#endif // DESCRIPTORSET_H