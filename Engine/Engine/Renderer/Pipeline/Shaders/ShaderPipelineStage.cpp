#include "ShaderPipelineStage.h"

#include "../../../Utils/Logger.h"
#include "Shader.h"
#include "DescriptorSetLayout.h"

#include <SPIRV-Reflect/spirv_reflect.h>

ShaderPipelineStage::ShaderPipelineStage() :
	vertexShader(nullptr),
	fragmentShader(nullptr),
	tesselationControlShader(nullptr),
	tesselationEvalShader(nullptr),
	geometryShader(nullptr),
	layout(new DescriptorSetLayout())
{
}

ShaderPipelineStage::~ShaderPipelineStage()
{
	delete layout;

	if (vertexShader != nullptr)
	{
		delete vertexShader;
	}

	if (fragmentShader != nullptr)
	{
		delete fragmentShader;
	}

	if (tesselationControlShader != nullptr)
	{
		delete tesselationControlShader;
	}

	if (tesselationEvalShader != nullptr)
	{
		delete tesselationEvalShader;
	}

	if (geometryShader != nullptr)
	{
		delete geometryShader;
	}
}

void ShaderPipelineStage::AddShader(const VkShaderStageFlagBits& shaderStageFlag, Shader* const shader)
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

	switch (shaderStageFlag)
	{
	case VK_SHADER_STAGE_VERTEX_BIT:
		if (vertexShader != nullptr)
		{
			delete vertexShader;
		}
		vertexShader = shader;
		break;
	case VK_SHADER_STAGE_FRAGMENT_BIT:
		if (fragmentShader != nullptr)
		{
			delete fragmentShader;
		}
		fragmentShader = shader;
		break;
	case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
		if (tesselationControlShader != nullptr)
		{
			delete tesselationControlShader;
		}
		tesselationControlShader = shader;
		break;
	case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
		if (tesselationEvalShader != nullptr)
		{
			delete tesselationEvalShader;
		}
		tesselationEvalShader = shader;
		break;
	case VK_SHADER_STAGE_GEOMETRY_BIT:
		if (geometryShader != nullptr)
		{
			delete geometryShader;
		}
		geometryShader = shader;
		break;
	default:
		break;
	}
}

Shader* const ShaderPipelineStage::GetShader(const VkShaderStageFlagBits& shaderStage) const
{
	switch (shaderStage)
	{
	case VK_SHADER_STAGE_VERTEX_BIT:
		return vertexShader;
	case VK_SHADER_STAGE_FRAGMENT_BIT:
		return fragmentShader;
	case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
		return tesselationControlShader;
	case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
		return tesselationEvalShader;
	case VK_SHADER_STAGE_GEOMETRY_BIT:
		return geometryShader;
	default:
		return nullptr;
	}
}

const std::vector<VkPipelineShaderStageCreateInfo>& ShaderPipelineStage::operator*() const
{
	return shaderStages;
}

void ShaderPipelineStage::CreateDescriptorSetLayout()
{
	auto addLayoutBindings = [this](const VkShaderStageFlagBits& shaderStage)
	{
		Shader* const shaderModule = GetShader(shaderStage);

		if (shaderModule == nullptr)
		{
			return;
		}

		SpvReflectShaderModule spvShaderModule;
		SpvReflectResult result = spvReflectCreateShaderModule(shaderModule->GetFileBuffer().size(), shaderModule->GetFileBuffer().data(), &spvShaderModule);

		if (result == SPV_REFLECT_RESULT_SUCCESS)
		{
			unsigned int descriptorBindingCount = 0;
			spvReflectEnumerateDescriptorBindings(&spvShaderModule, &descriptorBindingCount, nullptr);

			std::vector<SpvReflectDescriptorBinding*> descriptorBindings(descriptorBindingCount);
			result = spvReflectEnumerateDescriptorBindings(&spvShaderModule, &descriptorBindingCount, descriptorBindings.data());

			if (result == SPV_REFLECT_RESULT_SUCCESS)
			{
				for (SpvReflectDescriptorBinding* const binding : descriptorBindings)
				{
					VkDescriptorSetLayoutBinding layoutBinding{};
					layoutBinding.binding = binding->binding;
					layoutBinding.descriptorCount = 1;
					layoutBinding.stageFlags = shaderStage;
					layoutBinding.descriptorType = static_cast<VkDescriptorType>(binding->descriptor_type);
					layout->AddLayoutBinding(layoutBinding);
				}
			}
		}
	};

	addLayoutBindings(VK_SHADER_STAGE_VERTEX_BIT);
	addLayoutBindings(VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT);
	addLayoutBindings(VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);
	addLayoutBindings(VK_SHADER_STAGE_GEOMETRY_BIT);
	addLayoutBindings(VK_SHADER_STAGE_FRAGMENT_BIT);
	layout->Finalize();
}

const DescriptorSetLayout& ShaderPipelineStage::GetDescriptorSetLayout() const
{
	return *layout;
}
