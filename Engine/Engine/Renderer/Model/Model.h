#ifndef MODEL_H
#define MODEL_H

#include "Vertex.h"

#include <string>
#include <vector>

class Pose;
class Clip;
struct cgltf_data;

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

protected:

private:

	void LoadRestPose(cgltf_data* data);

	void LoadJointNames(cgltf_data* data);

	void LoadAnimationClips(cgltf_data* data);

	std::vector<Vertex> vertices;

	std::vector<unsigned int> indices;

	Pose* restPose;

	std::vector<std::string> jointNames;

	std::vector<Clip> animationClips;

};

#endif // MODEL_H