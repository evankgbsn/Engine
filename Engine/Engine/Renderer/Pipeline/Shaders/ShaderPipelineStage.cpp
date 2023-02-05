#include "ShaderPipelineStage.h"

#include "../../../Utils/Logger.h"
#include "Shader.h"

ShaderPipelineStage::ShaderPipelineStage()
{
}

ShaderPipelineStage::~ShaderPipelineStage()
{
}

void ShaderPipelineStage::AddShader(const VkShaderStageFlagBits& shaderStageFlag, const Shader* const shader)
{
	unsigned int i = 0;
	for (unsigned int i = 0; i < shaderStages.size(); ++i)
	{
		if (shaderStages[i].stage == shaderStageFlag)
		{
			Logger::Log(std::string("Replacing an existing shader in shader stage: ") + std::to_string(shaderStageFlag) + std::string(" with ") + shader->GetFileName(), Logger::Category::Warning);
			shaderStages[i].module = **shader;
			return;
		}
	}

	VkPipelineShaderStageCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	createInfo.stage = shaderStageFlag;
	createInfo.module = **shader;
	createInfo.pName = "main";

	shaderStages.push_back(createInfo);

}

const std::vector<VkPipelineShaderStageCreateInfo>& ShaderPipelineStage::operator*() const
{
	return shaderStages;
}
