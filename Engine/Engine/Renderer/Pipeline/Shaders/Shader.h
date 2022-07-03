#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <vector>

#include <vulkan/vulkan.h>

class VulkanPhysicalDevice;

class Shader
{
public:

	Shader(const std::string& fileName, VulkanPhysicalDevice* device);

	const VkShaderModule& operator*() const { return shaderModule; };

	~Shader();

	const std::string& GetFileName() const { return shaderFileName; };

	VulkanPhysicalDevice* const GetOwnerVulkanPhysicalDevice() const { return vulkanPhysicalDevice; };

	const std::vector<char>& GetFileBuffer() const { return buffer; };

private:

	Shader() = delete;

	Shader(const Shader&) = delete;

	Shader& operator=(const Shader&) = delete;

	Shader(const Shader&&) = delete;

	Shader& operator=(const Shader&&) = delete;

private:

	// The shader file as a char array.
	std::vector<char> buffer = std::vector<char>();

	// The vulkan shader module.
	VkShaderModule shaderModule = VK_NULL_HANDLE;

	// The device this shader module was created with.
	VulkanPhysicalDevice* const vulkanPhysicalDevice;

	// The name of the shader file.
	const std::string shaderFileName;
};

#endif // SHADER_H