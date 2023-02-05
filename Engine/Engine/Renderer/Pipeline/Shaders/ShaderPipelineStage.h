#ifndef SHADER_MODULE_H
#define SHADER_MODULE_H

#include <vector>

#include <vulkan/vulkan.h>

class Shader;

class ShaderPipelineStage
{

public:

	ShaderPipelineStage();

	~ShaderPipelineStage();

	void AddShader(const VkShaderStageFlagBits& shaderStage, const Shader* const);

	const std::vector<VkPipelineShaderStageCreateInfo>& operator*() const;

private:

	ShaderPipelineStage(const ShaderPipelineStage&) = delete;

	ShaderPipelineStage& operator=(const ShaderPipelineStage&) = delete;

	ShaderPipelineStage(const ShaderPipelineStage&&) = delete;

	ShaderPipelineStage& operator=(const ShaderPipelineStage&&) = delete;

	std::vector<VkPipelineShaderStageCreateInfo> shaderStages = std::vector<VkPipelineShaderStageCreateInfo>();
};

#endif // SHADER_MODULE_H