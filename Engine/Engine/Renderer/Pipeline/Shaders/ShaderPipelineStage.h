#ifndef SHADER_MODULE_H
#define SHADER_MODULE_H

#include <vector>

#include <vulkan/vulkan.h>

class Shader;

class ShaderPipelineStage
{

public:

	ShaderPipelineStage(Shader* vertex, Shader* fragment, Shader* geometry = nullptr, Shader* tesselationEvaluation = nullptr, Shader* tesselationControl = nullptr);

	~ShaderPipelineStage();

	const std::vector<VkPipelineShaderStageCreateInfo>& operator*() const;

private:

	ShaderPipelineStage() = delete;

	ShaderPipelineStage(const ShaderPipelineStage&) = delete;

	ShaderPipelineStage& operator=(const ShaderPipelineStage&) = delete;

	ShaderPipelineStage(const ShaderPipelineStage&&) = delete;

	ShaderPipelineStage& operator=(const ShaderPipelineStage&&) = delete;

	Shader* const vertex;

	Shader* const fragment;

	Shader* const geometry;

	Shader* const tesselationEvaluation;

	Shader* const tesselationControl;

	std::vector<VkPipelineShaderStageCreateInfo> shaderStages = std::vector<VkPipelineShaderStageCreateInfo>();
};

#endif // SHADER_MODULE_H