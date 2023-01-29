#include "Model.h"

#include "../../Utils/Logger.h"
#include "../../Math/Transform.h"
#include "../../Animation/Pose.h"

#pragma warning(disable : 4996)
#define _CRT_SECURE_NO_WARNINGS
#define CGLTF_IMPLEMENTATION
#include <cgltf/cgltf.h>

namespace GLTFHelpers
{
	Math::Transform GetLocalTransform(cgltf_node& n);
	int GetNodeIndex(cgltf_node* const target, cgltf_node* const allNodes, unsigned int numNodes);
};

Model::Model() :
	vertices(std::vector<Vertex>())
{
	// Default rectangle.
	vertices = {
		Vertex(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
		Vertex(glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
		Vertex(glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f)),
		Vertex(glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f))
	};

	indices = { 0,1,2,2,3,0 };
}

Model::Model(const std::vector<Vertex>& v, const std::vector<unsigned int>& i) :
	vertices(v),
	indices(i)
{
}

Model::Model(const std::string& path)
{
	cgltf_options options = {};
	cgltf_data* data = NULL;
	
	cgltf_result result = cgltf_parse_file(&options, path.c_str(), &data);
	
	if (result != cgltf_result_success)
	{
		Logger::Log(std::string("Failed to load Model from gltf file.") + path, Logger::Category::Error);
		return;
	}

	result = cgltf_load_buffers(&options, data, path.c_str());

	if (result != cgltf_result_success)
	{
		Logger::Log(std::string("Failed to load Model from gltf file.") + path, Logger::Category::Error);
		return;
	}

	result = cgltf_validate(data);

	if (result != cgltf_result_success)
	{
		Logger::Log(std::string("Failed to load Model from gltf file.") + path, Logger::Category::Error);
		return;
	}

	cgltf_free(data);
}

Model::~Model()
{
}

const std::vector<Vertex>& Model::GetVertices() const
{
	return vertices;
}

const std::vector<unsigned int>& Model::GetIndices() const
{
	return indices;
}

void Model::LoadRestPose(cgltf_data* data)
{
	unsigned int boneCount = data->nodes_count;
	restPose = new Pose(boneCount);

	for (unsigned int i = 0; i < boneCount; i++)
	{
		restPose->SetLocalTransform(i, GLTFHelpers::GetLocalTransform(data->nodes[i]));
		restPose->SetParent(i, GLTFHelpers::GetNodeIndex(&(data->nodes[i]), data->nodes, boneCount));
	}
}

void Model::LoadJointNames(cgltf_data* data)
{
	unsigned int boneCount = data->nodes_count;
	jointNames.resize(boneCount);

	for (unsigned int i = 0; i < boneCount; i++)
	{
		if (data->nodes[i].name == nullptr)
		{
			jointNames[i] = "No name";
		}
		else
		{
			jointNames[i] = data->nodes[i].name;
		}
	}
}


Math::Transform GLTFHelpers::GetLocalTransform(cgltf_node& n)
{
	Math::Transform result;

	if (n.has_matrix)
	{
		result = Math::Transform::FromMat4(glm::mat4(n.matrix[0], n.matrix[1], n.matrix[2], n.matrix[3],
												     n.matrix[4], n.matrix[5], n.matrix[6], n.matrix[7],
												     n.matrix[8], n.matrix[9], n.matrix[10], n.matrix[11],
												     n.matrix[12], n.matrix[13], n.matrix[14], n.matrix[15]));
	}

	if (n.has_translation)
	{
		result.Position() = glm::vec3(n.translation[0], n.translation[1], n.translation[2]);
	}

	if (n.has_rotation)
	{
		result.Rotation() = glm::quat(n.rotation[0], n.rotation[1], n.rotation[2], n.rotation[3]);
	}

	if (n.has_scale)
	{
		result.Scale() = glm::vec3(n.scale[0], n.scale[1], n.scale[2]);
	}

	return result;
}

int GLTFHelpers::GetNodeIndex(cgltf_node* const target, cgltf_node* const allNodes, unsigned int numNodes)
{
	if (target == nullptr)
	{
		return -1;
	}

	for (unsigned int i = 0; i < numNodes; i++)
	{
		if (target == &allNodes[i])
		{
			return static_cast<int>(i);
		}
	}

	return -1;
}
