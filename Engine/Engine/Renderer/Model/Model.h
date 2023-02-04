#ifndef MODEL_H
#define MODEL_H

#include "Vertex.h"

#include "../../Animation/Clip.h"

#include <string>
#include <vector>

class Pose;
class Armature;

struct cgltf_data;
struct cgltf_attribute;
struct cgltf_skin;
struct cgltf_node;

class Model
{

public:

	Model();

	Model(const std::vector<Vertex>& verices, const std::vector<unsigned int>& indices);

	Model(const std::string& path);

	~Model();

	Model(const Model&) = delete;

	Model& operator=(const Model&) = delete;

	Model(Model&&) = delete;

	Model& operator=(Model&&) = delete;

	const std::vector<Vertex>& GetVertices() const;

	const std::vector<unsigned int>& GetIndices() const;

	Armature* const GetArmature() const;

	std::vector<Clip>& GetAnimationClips();

	void CPUSkin(Armature& armature, Pose& pose);

protected:

private:

	void CPUSkinMatrices(Armature& armature, Pose& pose);

	void LoadAnimationClips(cgltf_data* data);

	void ModelFromAttribute(cgltf_attribute& attribute, cgltf_skin* skin, cgltf_node* nodes, unsigned int nodeCount);

	void LoadMeshFromGLTF(cgltf_data* data);

	std::vector<Vertex> vertices;

	std::vector<unsigned int> indices;

	Armature* armature;

	std::vector<Clip> animationClips;

	std::vector<glm::vec3> skinnedPosition;

	std::vector<glm::vec3> skinnedNormal;

	std::vector<glm::mat4> posePalette;

};

#endif // MODEL_H