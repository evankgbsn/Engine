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
#include "../Images/Texture.h"
#include "../Cameras//Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

GraphicsObject::GraphicsObject() :
	mvp(),
	model(ModelManager::GetModel("DefaultRectangle")),
	modelVertexBuffer(new VertexBuffer(static_cast<unsigned int>(sizeof(Vertex) * model->GetVertices().size()))),
	modelIndexBuffer(new IndexBuffer(static_cast<unsigned int>(sizeof(unsigned int) * model->GetIndices().size()))),
	mvpUniformBuffers(std::vector<UniformBuffer*>()),
	descriptorSets(std::vector<DescriptorSet*>()),
	texture(new Texture())
{
	CreateUniformBuffers();
	InitializeBuffers();
	CreateDescriptorSets();
}

GraphicsObject::GraphicsObject(Model* const m) :
	mvp(),
	model(m),
	modelVertexBuffer(new VertexBuffer(static_cast<unsigned int>(sizeof(Vertex) * model->GetVertices().size()))),
	modelIndexBuffer(new IndexBuffer(static_cast<unsigned int>(sizeof(unsigned int) * model->GetIndices().size()))),
	mvpUniformBuffers(std::vector<UniformBuffer*>()),
	descriptorSets(std::vector<DescriptorSet*>()),
	texture(new Texture())
{
	CreateUniformBuffers();
	InitializeBuffers();
	CreateDescriptorSets();
}

GraphicsObject::~GraphicsObject()
{
	delete modelIndexBuffer;
	delete modelVertexBuffer;

	for (unsigned int i = 0; i < descriptorSets.size(); i++)
	{
		delete descriptorSets[i];
	}

	for (unsigned int i = 0; i < mvpUniformBuffers.size(); i++)
	{
		mvpUniformBuffers[i]->Unmap();
		delete mvpUniformBuffers[i];
	}

	mvpUniformBuffers.clear();

	delete texture;
}

const DescriptorSet& GraphicsObject::GetDescriptorSet(unsigned int index) const
{
	return *descriptorSets[index];
}

const UniformBuffer& GraphicsObject::GetMVPUniformBuffer(unsigned int index) const
{
	return *mvpUniformBuffers[index];
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

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	GraphicsObject::MVPUniformBuffer ubo{};
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(30.0f), glm::vec3(0.0f, 0.0f, .2f));

	Camera cam(Camera::Type::PERSPECTIVE, WindowManager::GetWindow("MainWindow"));
	cam.SetPosition(glm::vec3(0.0f, 5.0f, 25.0f));

	ubo.view = cam.GetView();
	ubo.projection = cam.GetProjection();
	ubo.projection[1][1] *= -1;

	mvpUniformBuffers[index]->SetData(&ubo);
}

void GraphicsObject::SlowUpdate(unsigned int index)
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	GraphicsObject::MVPUniformBuffer ubo{};
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(0.0f), glm::vec3(0.0f, 0.0f, .2f));

	Camera cam(Camera::Type::PERSPECTIVE, WindowManager::GetWindow("MainWindow"));
	cam.SetPosition(glm::vec3(0.0f, 5.0f, 25.0f));

	ubo.view = cam.GetView(); 
	ubo.projection = cam.GetProjection(); 
	ubo.projection[1][1] *= -1;
	mvpUniformBuffers[index]->SetData(&ubo);
}

void GraphicsObject::CreateUniformBuffers()
{
	Window* const mainWindow = WindowManager::GetWindow("MainWindow");

	for (unsigned int i = 0; i < mainWindow->GetSwapChainImageCount(); i++)
	{
		UniformBuffer* const newUniformBuffer = new UniformBuffer(sizeof(mvp), 0);
		newUniformBuffer->PersistentMap();
		mvpUniformBuffers.push_back(newUniformBuffer);
	}
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

	DescriptorSetManager::CreateDescriptorSets("Descriptors", mvpUniformBuffers, texture->GetImage(), descriptorSets);
}
