#include "ShaderPipelineStage.h"

#include "Shader.h"

ShaderPipelineStage::ShaderPipelineStage(Shader* vert, Shader* frag, Shader* geo, Shader* tessE, Shader* tessC) :
	vertex(vert),
	fragment(frag),
	geometry(geo),
	tesselationEvaluation(tessE),
	tesselationControl(tessC)
{
	shaderStages.resize(2);

	VkPipelineShaderStageCreateInfo& vertCreateInfo = shaderStages[0];
	vertCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertCreateInfo.module = (*vertex)();
	vertCreateInfo.pName = "main";

	VkPipelineShaderStageCreateInfo& fragCreateInfo = shaderStages[1];
	fragCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragCreateInfo.module = (*fragment)();
	fragCreateInfo.pName = "main";
}

ShaderPipelineStage::~ShaderPipelineStage()
{
}

std::vector<VkPipelineShaderStageCreateInfo>& ShaderPipelineStage::GetShaderStages()
{
	return shaderStages;
}
