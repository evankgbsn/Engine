#include "GraphicsObject.h"

#include "../Model/ModelManager.h"
#include "../Model/Model.h"
#include "../Windows/WindowManager.h"
#include "../Windows/Window.h"
#include "../Memory/UniformBuffer.h"
#include "../Pipeline/Shaders/DescriptorSet.h"
#include "../Pipeline/Shaders/DescriptorSetManager.h"
#include "../Memory/VertexBuffer.h"
#include "../Memory/IndexBuffer.h"
#include "../Memory/StagingBuffer.h"
#include "../Memory/Image.h"
#include "../Images/Texture.h"
#include "../Cameras/CameraManager.h"
#include "../../Animation/Armature.h"
#include "../../Animation/Clip.h"
#include "../../Time/TimeManager.h"
#include "GraphicsObjectManager.h"

#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

GraphicsObject::GraphicsObject() :
	model(ModelManager::GetModel("DefaultRectangle")),
	modelVertexBuffer(new VertexBuffer(static_cast<unsigned int>(sizeof(Vertex) * model->GetVertices().size()))),
	modelIndexBuffer(new IndexBuffer(static_cast<unsigned int>(sizeof(unsigned int) * model->GetIndices().size()))),
	uniformBuffers(std::vector<UniformBuffer*>()),
	descriptorSet(nullptr),
	textures(std::vector<Texture*>())
{
	InitializeBuffers();
}

GraphicsObject::GraphicsObject(Model* const m) :
	model(m),
	modelVertexBuffer(new VertexBuffer(static_cast<unsigned int>(sizeof(Vertex) * model->GetVertices().size()))),
	modelIndexBuffer(new IndexBuffer(static_cast<unsigned int>(sizeof(unsigned int) * model->GetIndices().size()))),
	uniformBuffers(std::vector<UniformBuffer*>()),
	descriptorSet(nullptr),
	textures(std::vector<Texture*>())
{
	InitializeBuffers();
}

GraphicsObject::~GraphicsObject()
{
	delete modelIndexBuffer;
	delete modelVertexBuffer;

	delete descriptorSet;

	for (unsigned int i = 0; i < uniformBuffers.size(); i++)
	{
		uniformBuffers[i]->Unmap();
		delete uniformBuffers[i];
	}

	uniformBuffers.clear();

	for (unsigned int i = 0; i < textures.size(); i++)
	{
		delete textures[i];
	}
}

const DescriptorSet& GraphicsObject::GetDescriptorSet() const
{
	return *descriptorSet;
}

const VertexBuffer& GraphicsObject::GetVertexBuffer() const
{
	return *modelVertexBuffer;
}

const IndexBuffer& GraphicsObject::GetIndexBuffer() const
{
	return *modelIndexBuffer;
}

const Model* const GraphicsObject::GetModel() const
{
	return model;
}

const UniformBuffer* const GraphicsObject::GetUniformBuffer(unsigned int binding) const
{
	for (UniformBuffer* const buffer : uniformBuffers)
	{
		if (buffer->Binding() == binding)
		{
			return buffer;
		}
	}

	return nullptr;
}

const Image* const GraphicsObject::GetImage(unsigned int binding) const
{
	for (Texture* texture : textures)
	{
		if (texture->GetImage().Binding() == binding)
		{
			return &texture->GetImage();
		}
	}
	return nullptr;
}

void GraphicsObject::InitializeDescriptorSets()
{
	CreateUniformBuffers();
	CreateTextures();
	CreateDescriptorSets();
}

void GraphicsObject::InitializeBuffers()
{
	StagingBuffer vertexStagingBuffer(modelVertexBuffer->Size());
	vertexStagingBuffer.Map((void*)model->GetVertices().data(), modelVertexBuffer->Size());
	modelVertexBuffer->CopyFrom(vertexStagingBuffer);

	StagingBuffer indexStagingBuffer(modelIndexBuffer->Size());
	indexStagingBuffer.Map((void*)model->GetIndices().data(), modelIndexBuffer->Size());
	modelIndexBuffer->CopyFrom(indexStagingBuffer);
}

void GraphicsObject::CreateDescriptorSets()
{
	Window* const mainWindow = WindowManager::GetWindow("MainWindow");

	const ShaderPipelineStage* const shaderPipelineStage = GraphicsObjectManager::GetShaderPipelineStage("TexturedAnimated");
	if (shaderPipelineStage != nullptr)
	{
		descriptorSet = DescriptorSetManager::CreateDescriptorSetFromShader("Descriptors", *shaderPipelineStage, this);
	}
}
