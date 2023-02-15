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
	mvp(),
	model(ModelManager::GetModel("DefaultRectangle")),
	modelVertexBuffer(new VertexBuffer(static_cast<unsigned int>(sizeof(Vertex) * model->GetVertices().size()))),
	modelIndexBuffer(new IndexBuffer(static_cast<unsigned int>(sizeof(unsigned int) * model->GetIndices().size()))),
	uniformBuffers(std::vector<UniformBuffer*>()),
	descriptorSet(nullptr),
	textures(std::vector<Texture*>()),
	animatedPose(new Pose(model->GetArmature()->GetRestPose())),
	playback(0.0f)
{
	CreateUniformBuffers();
	CreateTextures();
	InitializeBuffers();
	CreateDescriptorSets();
}

GraphicsObject::GraphicsObject(Model* const m) :
	mvp(),
	model(m),
	modelVertexBuffer(new VertexBuffer(static_cast<unsigned int>(sizeof(Vertex) * model->GetVertices().size()))),
	modelIndexBuffer(new IndexBuffer(static_cast<unsigned int>(sizeof(unsigned int) * model->GetIndices().size()))),
	uniformBuffers(std::vector<UniformBuffer*>()),
	descriptorSet(nullptr),
	textures(std::vector<Texture*>()),
	animatedPose(new Pose(model->GetArmature()->GetRestPose())),
	playback(0.0f)
{
	CreateUniformBuffers();
	CreateTextures();
	InitializeBuffers();
	CreateDescriptorSets();
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

void GraphicsObject::Update(unsigned int index)
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	float time = TimeManager::DeltaTime();

	GraphicsObject::MVPUniformBuffer ubo{};
	ubo.model = glm::mat4(1.0f);

	const Camera& cam = CameraManager::GetCamera("MainCamera");

	ubo.view = cam.GetView();
	ubo.projection = cam.GetProjection();
	ubo.projection[1][1] *= -1;

	ubo.isSkinned = false;

	// Animation data
	const std::vector<glm::mat4>& invBindPose = model->GetArmature()->GetInvBindPose();
	for (unsigned int i = 0; i < invBindPose.size(); ++i)
	{
		ubo.invBindPose[i] = invBindPose[i];
	}
	
	std::vector<glm::mat4> posePalette(model->GetArmature()->GetRestPose().Size());

	std::vector<Clip>& clips = model->GetAnimationClips();
	if (clips.size() > 0)
	{
		playback = clips[1].Sample(*animatedPose, playback + 1.0f * time);
	}

	animatedPose->GetJointMatrices(posePalette);
	
	for (unsigned int i = 0; i < posePalette.size(); ++i)
	{
		ubo.pose[i] = posePalette[i];
	}

	uniformBuffers[0]->SetData(&ubo);
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

void GraphicsObject::CreateTextures()
{
	textures.push_back(new Texture());
}

void GraphicsObject::CreateUniformBuffers()
{
	UniformBuffer* mvpUniformBuffer = new UniformBuffer(sizeof(mvp), 0);
	mvpUniformBuffer->PersistentMap();
	uniformBuffers.push_back(mvpUniformBuffer);
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

	const ShaderPipelineStage* const shaderPipelineStage = GraphicsObjectManager::GetShaderPipelineStage("Animated");
	if (shaderPipelineStage != nullptr)
	{
		descriptorSet = DescriptorSetManager::CreateDescriptorSetFromShader("Descriptors", *shaderPipelineStage, this);
	}
}
