#include "ModelManager.h"

#include "../../Utils/Logger.h"
#include "Model.h"

#include <stdexcept>

ModelManager* ModelManager::instance = nullptr;

void ModelManager::Initialize()
{
	if (instance == nullptr)
	{
		instance = new ModelManager();
	}
}

void ModelManager::Terminate()
{
	if (instance != nullptr)
	{
		delete instance;
		instance = nullptr;
	}
}

Model* const ModelManager::LoadModel(const std::string& name, const std::string& path)
{
	if (instance == nullptr)
	{
		Logger::LogAndThrow(std::string("Calling ModelManager::LoadModel() before ModelManager::Initialize()."));
		return nullptr;
	}

	if (instance->models.find(name) == instance->models.end())
	{
		Logger::Log(std::string("Cannot load model with name ") + name + std::string(". This name is already being used."), Logger::Category::Warning);
		return nullptr;
	}

	return instance->models[name] = new Model(path);
}

Model* const ModelManager::LoadModel(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
{
	if (instance == nullptr)
	{
		Logger::LogAndThrow(std::string("Calling ModelManager::LoadModel() before ModelManager::Initialize()."));
		return nullptr;
	}

	if (instance->models.find(name) == instance->models.end())
	{
		Logger::Log(std::string("Cannot load model with name ") + name + std::string(". This name is already being used."), Logger::Category::Warning);
		return nullptr;
	}

	return instance->models[name] = new Model(vertices, indices);
}

Model* const ModelManager::GetModel(std::string modelName)
{
	if (instance == nullptr)
	{
		Logger::LogAndThrow(std::string("Calling ModelManager::GetModel() before ModelManager::Initialize()."));
		return nullptr;
	}

	if (instance->models.find(modelName) == instance->models.end())
	{
		Logger::Log(std::string("Could not find model ") + modelName, Logger::Category::Error);
		return nullptr;
	}

	return instance->models[modelName];
}

ModelManager::ModelManager() :
	models(std::unordered_map<std::string, Model*>())
{
	LoadDefaultModels();
}

ModelManager::~ModelManager()
{
	for (auto& model : models)
	{
		delete model.second;
	}
}

void ModelManager::LoadDefaultModels()
{
	models.insert(std::make_pair(std::string("DefaultRectangle"), new Model()));


	// Default rectangle.
	std::vector<Vertex> vertices = {
		Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
		Vertex(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
		Vertex(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f)),
		Vertex(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f))
	};

	std::vector<unsigned int> indices = { 0,1,2,2,3,0 };

	models.insert(std::make_pair(std::string("DefaultRectangleWithDepth"), new Model(vertices, indices)));

	models.insert(std::make_pair(std::string("DefaultCube"), new Model("../Engine/Engine/Renderer/Model/Woman.gltf")));
}
