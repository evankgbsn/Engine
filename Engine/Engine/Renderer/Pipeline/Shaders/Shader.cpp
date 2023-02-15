#include "Shader.h"

#include <fstream>
#include <stdexcept>

#include "../../../Utils/Logger.h"
#include "../../Vulkan/VulkanPhysicalDevice.h"

Shader::Shader(const std::filesystem::directory_entry& dirEntry, VulkanPhysicalDevice* device) :
	vulkanPhysicalDevice(device),
	shaderFileDirEntry(dirEntry)
{
	std::ifstream file(dirEntry.path().string(), std::ios::ate | std::ios::binary);
	
	if (!file.is_open())
	{
		Logger::Log(std::string("Could not read shader file ") + dirEntry.path().string(), Logger::Category::Error);
		throw std::runtime_error((std::string("Could not read shader file ") + dirEntry.path().string()).c_str());
		return;
	}

	size_t fileSize = (size_t)file.tellg();
	buffer.resize(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = buffer.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());

	VkResult result = vkCreateShaderModule(device->GetLogicalDevice(), &createInfo, nullptr, &shaderModule);

	if (result != VK_SUCCESS)
	{
		std::string error = std::string("Could not create vulkan shader module for ") + GetFileName();
		Logger::Log(std::string(error), Logger::Category::Error);
		throw std::runtime_error(error.c_str());
		return;
	}

	Logger::Log(std::string("Loaded shader ") + GetFileName(), Logger::Category::Success);
}

Shader::~Shader()
{
	vkDestroyShaderModule(vulkanPhysicalDevice->GetLogicalDevice(), shaderModule, nullptr);
}
