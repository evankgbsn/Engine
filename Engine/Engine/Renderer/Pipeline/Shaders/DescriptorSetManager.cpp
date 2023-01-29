#include "DescriptorSetManager.h"

#include "../../Vulkan/Utils/VulkanUtils.h"
#include "../../Renderer.h"
#include "../../Vulkan/VulkanPhysicalDevice.h"

#include "DescriptorSet.h"

DescriptorSetManager* DescriptorSetManager::instance = nullptr;

void DescriptorSetManager::Initialize()
{
	if (instance == nullptr)
	{
		instance = new DescriptorSetManager();
	}
	else
	{
		const char* message = "Calling DescriptorSetManager::Initialize() before DescriptorSetManager::Terminate().";
		Logger::Log(std::string(message));
		throw std::runtime_error(message);
	}
}

void DescriptorSetManager::Terminate()
{
	if (instance != nullptr)
	{
		delete instance;
	}
	else
	{
		const char* message = "Calling DescriptorSetManager::Terminate() before DescriptorSetManager::Initialize().";
		Logger::LogAndThrow(message);
	}
}

void DescriptorSetManager::CreateDescriptorSetPool(unsigned int poolSize, const std::string& poolName)
{
	if (instance == nullptr)
	{
		const char* message = "Calling DescriptorSetManager::Terminate() before DescriptorSetManager::Initialize().";
		Logger::LogAndThrow(message);
		return;
	}

	VkDescriptorPoolSize uniformPoolSizeStruct = {};
	uniformPoolSizeStruct.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uniformPoolSizeStruct.descriptorCount = poolSize;

	VkDescriptorPoolSize imagePoolSizeStruct = {};
	imagePoolSizeStruct.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	imagePoolSizeStruct.descriptorCount = poolSize;

	std::vector<VkDescriptorPoolSize> sizes = {uniformPoolSizeStruct, imagePoolSizeStruct};

	VkDescriptorPoolCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	createInfo.poolSizeCount = 2;
	createInfo.pPoolSizes = sizes.data();
	createInfo.maxSets = poolSize;

	VkDescriptorPool* pool = new VkDescriptorPool();

	VkResult result = vkCreateDescriptorPool(Renderer::GetVulkanPhysicalDevice()->GetLogicalDevice(), &createInfo, nullptr, pool);
	VulkanUtils::CheckResult(result,true, true, "Failed to create a descriptor pool");

	instance->descriptorSetPools[poolName] = std::make_pair(pool, sizes);
}

void DescriptorSetManager::CreateDescriptorSets(const std::string& poolName, const std::vector<UniformBuffer*>& uniformBuffers, const Image& image, std::vector<DescriptorSet*>& outDescriptorSets)
{
	for (unsigned int i = 0; i < uniformBuffers.size(); i++)
	{
		outDescriptorSets.push_back(new DescriptorSet(*(instance->descriptorSetPools[poolName].first), *(uniformBuffers[i]), image));
	}
}

DescriptorSetManager::DescriptorSetManager() :
	descriptorSetPools(std::unordered_map<std::string, std::pair<VkDescriptorPool*, std::vector<VkDescriptorPoolSize>>>())
{

}

DescriptorSetManager::~DescriptorSetManager()
{

	VkDevice& device = Renderer::GetVulkanPhysicalDevice()->GetLogicalDevice();

	for (auto& pool : descriptorSetPools)
	{
		vkDestroyDescriptorPool(device, *pool.second.first, nullptr);

		delete pool.second.first;
	}
}
