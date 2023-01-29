#include "Model.h"

#include "../../Utils/Logger.h"
#include "../../Math/Transform.h"
#include "../../Animation/Pose.h"
#include "../../Animation/Track.h"
#include "../../Animation/Clip.h"

#pragma warning(disable : 4996)
#define _CRT_SECURE_NO_WARNINGS
#define CGLTF_IMPLEMENTATION
#include <cgltf/cgltf.h>

namespace GLTFHelpers
{
	Math::Transform GetLocalTransform(cgltf_node& n);
	
	int GetNodeIndex(cgltf_node* const target, cgltf_node* const allNodes, unsigned int numNodes);
	
	void GetScalarValues(std::vector<float>& out, unsigned int compCount, const cgltf_accessor& inAccessor);

	template<typename T, unsigned int N>
	void TrackFromChannel(Track<T, N>& result, const cgltf_animation_channel& channel);

	Pose LoadRestPose(cgltf_data* data);

	Pose LoadBindPose(cgltf_data* const data);

	std::vector<std::string> LoadJointNames(cgltf_data* data);

	Armature LoadArmature(cgltf_data* const data);
};

Model::Model() :
	vertices(std::vector<Vertex>()),
	indices(std::vector<unsigned int>()),
	animationClips(std::vector<Clip>())

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
	indices(i),
	animationClips(std::vector<Clip>())
{
}

Model::Model(const std::string& path) :
	vertices(std::vector<Vertex>()),
	indices(std::vector<unsigned int>()),
	animationClips(std::vector<Clip>())
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

Pose GLTFHelpers::LoadRestPose(cgltf_data* data)
{
	unsigned int boneCount = static_cast<unsigned int>(data->nodes_count);
	Pose result(boneCount);

	for (unsigned int i = 0; i < boneCount; i++)
	{
		result.SetLocalTransform(i, GLTFHelpers::GetLocalTransform(data->nodes[i]));
		result.SetParent(i, GLTFHelpers::GetNodeIndex(&(data->nodes[i]), data->nodes, boneCount));
	}

	return result;
}

std::vector<std::string> GLTFHelpers::LoadJointNames(cgltf_data* data)
{
	unsigned int boneCount = static_cast<unsigned int>(data->nodes_count);
	std::vector<std::string> result(boneCount);

	for (unsigned int i = 0; i < boneCount; i++)
	{
		if (data->nodes[i].name == nullptr)
		{
			result[i] = "No name";
		}
		else
		{
			result[i] = data->nodes[i].name;
		}
	}
}

void Model::LoadAnimationClips(cgltf_data* data)
{
	unsigned int numClips = data->animations_count;
	unsigned int numNodes = data->nodes_count;
	animationClips.resize(numClips);

	for (unsigned int i = 0; i < numClips; i++)
	{
		animationClips[i].SetName(data->animations[i].name);

		unsigned int numChannels = data->animations->channels_count;

		for (unsigned int j = 0; j < numChannels; j++)
		{
			cgltf_animation_channel& channel = data->animations[i].channels[j];
			cgltf_node* target = channel.target_node;
			int nodeId = GLTFHelpers::GetNodeIndex(target, data->nodes, numNodes);

			switch (channel.target_path)
			{
			case cgltf_animation_path_type_translation:
				VectorTrack& transTrack = animationClips[i][nodeId].GetPositionTrack();
				GLTFHelpers::TrackFromChannel<glm::vec3, 3>(transTrack, channel);
				break;
			case cgltf_animation_path_type_rotation:
				QuaternionTrack& rotTrack = animationClips[i][nodeId].GetRotationTrack();
				GLTFHelpers::TrackFromChannel<glm::quat, 4>(rotTrack, channel);
				break;
			case cgltf_animation_path_type_scale:
				VectorTrack& scaleTrack = animationClips[i][nodeId].GetScaleTrack();
				GLTFHelpers::TrackFromChannel<glm::vec3, 3>(scaleTrack, channel);
				break;
			}
		}

		animationClips[i].RecalculateDuration();
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

void GLTFHelpers::GetScalarValues(std::vector<float>& out, unsigned int compCount, const cgltf_accessor& inAccessor)
{
	out.resize(inAccessor.count * compCount);
	for (cgltf_size i = 0; i < inAccessor.count; i++)
	{
		cgltf_accessor_read_float(&inAccessor, i, &out[i * compCount], compCount);
	}
}

template<typename T, unsigned int N>
void GLTFHelpers::TrackFromChannel(Track<T, N>& result, const cgltf_animation_channel& channel)
{
	cgltf_animation_sampler& sampler = *channel.sampler;
	Interpolation interpolation = Interpolation::Constant;
	
	switch(sampler.interpolation)
	{
		case cgltf_interpolation_type_linear:
			interpolation = Interpolation::Linear;
			break;
		case cgltf_interpolation_type_cubic_spline:
			interpolation = Interpolation::Cubic;
			break;
		default:
			break;
	}

	bool isSamplerCubic = interpolation == Interpolation::Cubic;

	result.SetInterpolation(interpolation);

	std::vector<float> time; // times
	GetScalarValues(time, 1, *sampler.input);

	std::vector<float> val; // values
	GetScalarValues(val, N, *sampler.output);

	unsigned int numFrames = sampler.input->count;
	unsigned int compCount = val.size() / time.size();
	result.SetSize(numFrames);

	for (unsigned int i = 0; i < numFrames; ++i)
	{
		int baseIndex = i * compCount;
		Frame<N>& frame = result[i];
		int offset = 0;
		frame.time = time[i];

		for (int comp = 0; comp < N; ++comp)
		{
			frame.inTangent[comp] = (isSamplerCubic) ? val[baseIndex + offset++] : 0.0f;
		}

		for (int comp = 0; comp < N; ++comp)
		{
			frame.value[comp] = val[baseIndex + offset++];
		}

		for (int comp = 0; comp < N; ++comp)
		{
			frame.outTangent[comp] = (isSamplerCubic) ? val[baseIndex + offset++] : 0.0f;
		}
	}
}

Pose GLTFHelpers::LoadBindPose(cgltf_data* const data)
{
	Pose restPose = GLTFHelpers::LoadRestPose(data);
	unsigned int numBones = restPose.Size();
	std::vector<Math::Transform> worldBindPose(numBones);

	for (unsigned int i = 0; i < numBones; ++i)
	{
		worldBindPose[i] = restPose.GetGlobalTransform(i);
	}

	unsigned int numSkins = data->skins_count;
	for (unsigned int i = 0; i < numSkins; ++i)
	{
		cgltf_skin* skin = &(data->skins[i]);
		std::vector<float> invBindAccessor;
		GLTFHelpers::GetScalarValues(invBindAccessor, 16, *skin->inverse_bind_matrices);

		unsigned int numJoints = skin->joints_count;
		for (unsigned int j = 0; j < numJoints; ++j)
		{
			// Read the inverse bind matrix of the joint.
			float* matrix = &(invBindAccessor[j * 16]);
			glm::mat4 invBindMatrix(
				matrix[0], matrix[1], matrix[2], matrix[3],
				matrix[4], matrix[5], matrix[6], matrix[7],
				matrix[8], matrix[9], matrix[10], matrix[11],
				matrix[12], matrix[13], matrix[14], matrix[15]
			);

			// Invert and convert to transform.
			glm::mat4 bindMatrix = glm::inverse(invBindMatrix);
			Math::Transform bindTransform = Math::Transform::FromMat4(bindMatrix);

			// Set that transform in the world bind pose.
			cgltf_node* jointNode = skin->joints[j];
			int jointIndex = GLTFHelpers::GetNodeIndex(jointNode, data->nodes, numBones);

			worldBindPose[jointIndex] = bindTransform;
		}
	}

	// Convert the world bind pose to a regular bind pose.
	Pose bindPose = restPose;
	for (unsigned int i = 0; i < numBones; ++i)
	{
		Math::Transform current = worldBindPose[i];
		int p = bindPose.GetParent(i);
		if (p >= 0)
		{
			// Bring into parent space.
			Math::Transform parent = worldBindPose[p];
			current = Math::Transform::Combine(Math::Transform::Inverse(parent), current);
		}
		bindPose.SetLocalTransform(i, current);
	}

	return bindPose;
}

Armature GLTFHelpers::LoadArmature(cgltf_data* const data)
{
	return Armature(LoadRestPose(data), LoadBindPose(data), LoadJointNames(data));
}
