#ifndef MODEL_H
#define MODEL_H

#include "Vertex.h"
#include "../../Animation/Armature.h"

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

	void LoadAnimationClips(cgltf_data* data);

	std::vector<Vertex> vertices;

	std::vector<unsigned int> indices;

	Armature armature;

	std::vector<Clip> animationClips;

};

#endif // MODEL_H