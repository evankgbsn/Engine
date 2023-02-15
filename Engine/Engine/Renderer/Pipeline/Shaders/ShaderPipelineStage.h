#ifndef SHADER_MODULE_H
#define SHADER_MODULE_H

#include <vector>

#include <vulkan/vulkan.h>

class Shader;
class DescriptorSetLayout;

class ShaderPipelineStage
{

public:

	ShaderPipelineStage();

	~ShaderPipelineStage();

	void AddShader(const VkShaderStageFlagBits& shaderStage, Shader* const);

	Shader* const GetShader(const VkShaderStageFlagBits& shaderStage) const;

	const std::vector<VkPipelineShaderStageCreateInfo>& operator*() const;

	void CreateDescriptorSetLayout();

	const DescriptorSetLayout& GetDescriptorSetLayout() const;

private:

	ShaderPipelineStage(const ShaderPipelineStage&) = delete;

	ShaderPipelineStage& operator=(const ShaderPipelineStage&) = delete;

	ShaderPipelineStage(const ShaderPipelineStage&&) = delete;

	ShaderPipelineStage& operator=(const ShaderPipelineStage&&) = delete;

	std::vector<VkPipelineShaderStageCreateInfo> shaderStages = std::vector<VkPipelineShaderStageCreateInfo>();

	Shader* vertexShader;

	Shader* fragmentShader;

	Shader* tesselationControlShader;

	Shader* tesselationEvalShader;

	Shader* geometryShader;

	DescriptorSetLayout* layout;
};

#endif // SHADER_MODULE_H