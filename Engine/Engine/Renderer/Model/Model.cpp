#include "Model.h"

#include "../../Utils/Logger.h"
#include "../../Animation/Armature.h"
#include "../../Animation/Pose.h"
#include "../../Animation/BakedAnimation.h"

#pragma warning(disable : 4996)
#define _CRT_SECURE_NO_WARNINGS
#define CGLTF_IMPLEMENTATION
#include <cgltf/cgltf.h>

namespace GLTFHelpers
{
	Math::Transform GetLocalTransform(cgltf_node& n);
	
	int GetNodeIndex(cgltf_node* const target, cgltf_node* const allNodes, unsigned int numNodes);
	
	void GetScalarValues(std::vector<float>& out, unsigned int compCount, const cgltf_accessor& inAccessor);

	template<typename T, size_t N>
	void TrackFromChannel(Track<T, N>& result, const cgltf_animation_channel& channel);

	Pose LoadRestPose(cgltf_data* data);

	Pose LoadBindPose(cgltf_data* const data, const Pose& restPose);

	std::vector<std::string> LoadJointNames(cgltf_data* data);

	Armature LoadArmature(cgltf_data* const data);
};

Model::Model() :
	vertices(std::vector<Vertex>()),
	indices(std::vector<unsigned int>()),
	animationClips(std::vector<Clip>()),
	armature(new Armature())
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
	animationClips(std::vector<Clip>()),
	armature(new Armature())
{
}

Model::Model(const std::string& path) :
	vertices(std::vector<Vertex>()),
	indices(std::vector<unsigned int>()),
	animationClips(std::vector<Clip>()),
	armature(new Armature())
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

	LoadMeshFromGLTF(data);
	*armature = GLTFHelpers::LoadArmature(data);
	LoadAnimationClips(data);
	cgltf_free(data);

	BakeAnimations();
}

Model::~Model()
{
	delete armature;
}

const std::vector<Vertex>& Model::GetVertices() const
{
	return vertices;
}

const std::vector<unsigned int>& Model::GetIndices() const
{
	return indices;
}

Armature* const Model::GetArmature() const
{
	return armature;
}

const std::vector<Clip>& Model::GetAnimationClips() const
{
	return animationClips;
}

Pose GLTFHelpers::LoadRestPose(cgltf_data* data)
{
	unsigned int boneCount = static_cast<unsigned int>(data->nodes_count);
	Pose result(boneCount);

	for (unsigned int i = 0; i < boneCount; ++i)
	{
		cgltf_node* node = &(data->nodes[i]);

		Math::Transform transform = GetLocalTransform(data->nodes[i]);
		result.SetLocalTransform(i, transform);

		int parent = GetNodeIndex(node->parent, data->nodes, boneCount);
		result.SetParent(i, parent);
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

	return result;
}

const BakedAnimation& Model::GetBakedAnimation(unsigned int index) const
{
	return bakedAnimations[index];
}

void Model::CPUSkin(Armature& armature, Pose& pose)
{
	unsigned int numVerts = static_cast<unsigned int>(vertices.size());
	if (numVerts == 0)
	{
		return;
	}

	skinnedPosition.resize(numVerts);
	skinnedNormal.resize(numVerts);

	Pose& bindPose = const_cast<Pose&>(armature.GetBindPose());

	for (unsigned int i = 0; i < numVerts; ++i)
	{
		glm::ivec4& joint = vertices[i].GetInfluences();
		glm::vec4& weight = vertices[i].GetWeights();

		Math::Transform skin0 = Math::Transform::Combine(pose[joint.x], Math::Transform::Inverse(bindPose[joint.x]));
		glm::vec3 p0 = Math::Transform::TransformPoint(skin0, vertices[i].GetPosition());
		glm::vec3 n0 = Math::Transform::TransformVector(skin0, vertices[i].GetNormal());

		Math::Transform skin1 = Math::Transform::Combine(pose[joint.y], Math::Transform::Inverse(bindPose[joint.y]));
		glm::vec3 p1 = Math::Transform::TransformPoint(skin1, vertices[i].GetPosition());
		glm::vec3 n1 = Math::Transform::TransformVector(skin1, vertices[i].GetNormal());

		Math::Transform skin2 = Math::Transform::Combine(pose[joint.z], Math::Transform::Inverse(bindPose[joint.z]));
		glm::vec3 p2 = Math::Transform::TransformPoint(skin2, vertices[i].GetPosition());
		glm::vec3 n2 = Math::Transform::TransformVector(skin2, vertices[i].GetNormal());

		Math::Transform skin3 = Math::Transform::Combine(pose[joint.w], Math::Transform::Inverse(bindPose[joint.w]));
		glm::vec3 p3 = Math::Transform::TransformPoint(skin3, vertices[i].GetPosition());
		glm::vec3 n3 = Math::Transform::TransformVector(skin3, vertices[i].GetNormal());

		vertices[i].GetPosition() = p0 * weight.x + p1 * weight.y + p2 * weight.z + p3 * weight.w;
		skinnedNormal[i] = n0 * weight.x + n1 * weight.y + n2 * weight.z + n3 * weight.w;
	}
}

void Model::CPUSkinMatrices(Armature& armature, Pose& pose)
{
	unsigned int numVerts = static_cast<unsigned int>(vertices.size());
	if (numVerts == 0)
	{
		return;
	}

	skinnedPosition.resize(numVerts);
	skinnedNormal.resize(numVerts);

	pose.GetJointMatrices(posePalette);
	std::vector<glm::mat4> invPosePalette = armature.GetInvBindPose();

	for (unsigned int i = 0; i < numVerts; ++i)
	{
		glm::ivec4& j = vertices[i].GetInfluences();
		glm::vec4& w = vertices[i].GetWeights();

		glm::mat4 m0 = (posePalette[j.x] * invPosePalette[j.x]) * w.x;
		glm::mat4 m1 = (posePalette[j.y] * invPosePalette[j.y]) * w.y;
		glm::mat4 m2 = (posePalette[j.z] * invPosePalette[j.z]) * w.z;
		glm::mat4 m3 = (posePalette[j.w] * invPosePalette[j.w]) * w.w;

		glm::mat4 skin = m0 + m1 + m2 + m3;

		skinnedPosition[i] = Math::Transform::TransformPoint(Math::Transform::FromMat4(skin), vertices[i].GetPosition());
		skinnedNormal[i] = Math::Transform::TransformVector(Math::Transform::FromMat4(skin), vertices[i].GetNormal());
	}
}

void Model::LoadAnimationClips(cgltf_data* data)
{
	unsigned int numClips = static_cast<unsigned int>(data->animations_count);
	unsigned int numNodes = static_cast<unsigned int>(data->nodes_count);
	animationClips.resize(numClips);

	for (unsigned int i = 0; i < numClips; i++)
	{
		animationClips[i].SetName(data->animations[i].name);

		unsigned int numChannels = static_cast<unsigned int>(data->animations->channels_count);

		for (unsigned int j = 0; j < numChannels; j++)
		{
			cgltf_animation_channel& channel = data->animations[i].channels[j];
			cgltf_node* target = channel.target_node;
			int nodeId = GLTFHelpers::GetNodeIndex(target, data->nodes, numNodes);

			if (channel.target_path == cgltf_animation_path_type_translation)
			{
				VectorTrack& transTrack = animationClips[i][nodeId].GetPositionTrack();
				GLTFHelpers::TrackFromChannel<glm::vec3, 3U>(transTrack, channel);
			}
			else if (channel.target_path == cgltf_animation_path_type_rotation)
			{
				QuaternionTrack& rotTrack = animationClips[i][nodeId].GetRotationTrack();
				GLTFHelpers::TrackFromChannel<glm::quat, 4U>(rotTrack, channel);
			}
			else if(channel.target_path == cgltf_animation_path_type_scale)
			{
				VectorTrack& scaleTrack = animationClips[i][nodeId].GetScaleTrack();
				GLTFHelpers::TrackFromChannel<glm::vec3, 3U>(scaleTrack, channel);
			}
		}

		animationClips[i].RecalculateDuration();
	}

}

void Model::BakeAnimations()
{
	bakedAnimations.reserve(animationClips.size());
	for (unsigned int i = 0; i < animationClips.size(); i++)
	{
		bakedAnimations.push_back(BakedAnimation(&animationClips[i], GetArmature()));
	}
}

void Model::ModelFromAttribute(cgltf_attribute& attribute, cgltf_skin* skin, cgltf_node* nodes, unsigned int nodeCount)
{
	cgltf_attribute_type attribType = attribute.type;
	cgltf_accessor& accessor = *attribute.data;

	unsigned int componentCount = 0;
	if (accessor.type == cgltf_type_vec2)
	{
		componentCount = 2;
	}
	else if (accessor.type == cgltf_type_vec3)
	{
		componentCount = 3;
	}
	else if (accessor.type == cgltf_type_vec4)
	{
		componentCount = 4;
	}

	std::vector<float> values;
	GLTFHelpers::GetScalarValues(values, componentCount, accessor);
	unsigned int accessorCount = static_cast<unsigned int>(accessor.count);

	if (vertices.size() == 0)
	{
		vertices.resize(accessorCount);
	}

	for (unsigned int i = 0; i < accessorCount; ++i)
	{
		unsigned int index = i * componentCount;

		switch (attribType)
		{
		case cgltf_attribute_type_position:
			vertices[i].GetPosition() = glm::vec3(values[index + 0], values[index + 1], values[index + 2]);
			break;
		case cgltf_attribute_type_texcoord:
			vertices[i].GetUV() = glm::vec2(values[index + 0], values[index + 1]);
			break;
		case cgltf_attribute_type_weights:
			vertices[i].GetWeights() = glm::vec4(values[index + 0], values[index + 1], values[index + 2], values[index + 3]);
			break;
		case cgltf_attribute_type_normal:
			vertices[i].GetNormal() = glm::vec3(values[index + 0], values[index + 1], values[index + 2]);
			if (glm::length2(vertices[i].GetNormal()) < 0.000001f)
			{
				vertices[i].GetNormal() = glm::vec3(0, 1, 0);
			}
			break;
		case cgltf_attribute_type_joints:
			/*
				These indices are skin relative. This function has no information about
				the skin that is being parsed. Add +0.5f to round since we can't read integers.
			*/

			glm::ivec4& influences = vertices[i].GetInfluences();
			influences.x = (int)(values[index + 0] + 0.5f);
			influences.y = (int)(values[index + 1] + 0.5f);
			influences.z = (int)(values[index + 2] + 0.5f);
			influences.w = (int)(values[index + 3] + 0.5f);

			// Use the GetNodeIndex helper function to convert the joint indices so that they go from being relative to the joints array to being relative to the armature heirarchy.
			influences.x = GLTFHelpers::GetNodeIndex(skin->joints[influences.x], nodes, nodeCount);
			influences.y = GLTFHelpers::GetNodeIndex(skin->joints[influences.y], nodes, nodeCount);
			influences.z = GLTFHelpers::GetNodeIndex(skin->joints[influences.z], nodes, nodeCount);
			influences.w = GLTFHelpers::GetNodeIndex(skin->joints[influences.w], nodes, nodeCount);

			// Make sure that even the invalid nodes have a value of 0. Any negative joint indices will break the skinning implementation.
			influences.x = std::max(0, influences.x);
			influences.y = std::max(0, influences.y);
			influences.z = std::max(0, influences.z);
			influences.w = std::max(0, influences.w);

			break;
		}
	}
}

void Model::LoadMeshFromGLTF(cgltf_data* data)
{
	cgltf_node* nodes = data->nodes;
	unsigned int nodeCount = static_cast<unsigned int>(data->nodes_count);

	for (unsigned int i = 0; i < nodeCount; ++i)
	{
		cgltf_node* node = &nodes[i];
		if (node->mesh == nullptr)
		{
			continue;
		}

		unsigned int numPrims = static_cast<unsigned int>(node->mesh->primitives_count);
		for (unsigned int j = 0; j < numPrims; ++j)
		{
			cgltf_primitive* primitive = &node->mesh->primitives[j];
			
			unsigned int attributeCount = static_cast<unsigned int>(primitive->attributes_count);
			for (unsigned int k = 0; k < attributeCount; ++k)
			{
				cgltf_attribute* attribute = &primitive->attributes[k];
				ModelFromAttribute(*attribute, node->skin, nodes, nodeCount);
			}

			if (primitive->indices != nullptr)
			{
				unsigned int indicesCount = static_cast<unsigned int>(primitive->indices->count);
				indices.resize(indicesCount);
				for (unsigned int k = 0; k < indicesCount; ++k)
				{
					indices[k] = static_cast<unsigned int>(cgltf_accessor_read_index(primitive->indices, k));
				}
			}
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
		result.Rotation() = glm::quat(n.rotation[3], n.rotation[0], n.rotation[1], n.rotation[2]);
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

template<typename T, size_t N>
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
		frame.SetTime(time[i]);

		for (int comp = 0; comp < N; ++comp)
		{
			frame.GetInTangent()[comp] = (isSamplerCubic) ? val[baseIndex + offset++] : 0.0f;
		}

		for (int comp = 0; comp < N; ++comp)
		{
			frame.GetValue()[comp] = val[baseIndex + offset++];
		}

		for (int comp = 0; comp < N; ++comp)
		{
			frame.GetOutTangent()[comp] = (isSamplerCubic) ? val[baseIndex + offset++] : 0.0f;
		}
	}
}

Pose GLTFHelpers::LoadBindPose(cgltf_data* const data, const Pose& restPose)
{
	unsigned int numBones = restPose.Size();
	std::vector<Math::Transform> worldBindPose(numBones);
	
	for (unsigned int i = 0; i < numBones; ++i)
	{
		worldBindPose[i] = restPose.GetGlobalTransform(i);
	}
	
	unsigned int numSkins = static_cast<unsigned int>(data->skins_count);
	for (unsigned int i = 0; i < numSkins; ++i)
	{
		cgltf_skin* skin = &(data->skins[i]);
		std::vector<float> invBindAccessor;
		GLTFHelpers::GetScalarValues(invBindAccessor, 16, *skin->inverse_bind_matrices);
	
		unsigned int numJoints = static_cast<unsigned int>(skin->joints_count);
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
	Pose restPose = LoadRestPose(data);
	return Armature(restPose, LoadBindPose(data, restPose), LoadJointNames(data));
}
