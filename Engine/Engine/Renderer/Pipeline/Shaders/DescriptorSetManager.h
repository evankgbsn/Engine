#ifndef DESCRIPTORSETMANAGER_H
#define DESCRIPTORSETMANAGER_H

#include <vulkan/vulkan.h>
#include <unordered_map>
#include <string>

class DescriptorSet;
class UniformBuffer;
class Image;
class ShaderPipelineStage;
class GraphicsObject;

class DescriptorSetManager
{

public:

	static void Initialize();

	static void Terminate();

	static void CreateDescriptorSetPool(unsigned int poolSize, const std::string& poolName);

	static DescriptorSet* CreateDescriptorSetFromShader(const std::string& poolName, const ShaderPipelineStage& shader, GraphicsObject* const);

private:

	DescriptorSetManager();

	~DescriptorSetManager();

	DescriptorSetManager(const DescriptorSetManager&) = delete;

	DescriptorSetManager& operator=(const DescriptorSetManager&) = delete;

	DescriptorSetManager(DescriptorSetManager&&) = delete;

	DescriptorSetManager& operator=(DescriptorSetManager&&) = delete;

	static DescriptorSetManager* instance;
	
	std::unordered_map<std::string, std::pair<VkDescriptorPool*, std::vector<VkDescriptorPoolSize>>> descriptorSetPools;
};

#endif // DESCRIPTORSETMANAGER_H