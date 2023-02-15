#ifndef GRAPHICSOBJECT_H
#define GRAPHICSOBJECT_H

#include <glm/glm.hpp>
#include <vector>

class Model;
class UniformBuffer;
class DescriptorSet;
class DescriptorSetLayout;
class VertexBuffer;
class IndexBuffer;
class Texture;
class Image;
class Pose;

class GraphicsObject
{

public:

	GraphicsObject();

	GraphicsObject(Model* const model);

	~GraphicsObject();

	GraphicsObject(const GraphicsObject&) = delete;

	GraphicsObject& operator=(const GraphicsObject&) = delete;

	GraphicsObject(GraphicsObject&&) = delete;

	GraphicsObject& operator=(GraphicsObject&&) = delete;

	const DescriptorSet& GetDescriptorSet() const;

	const VertexBuffer& GetVertexBuffer() const;

	const IndexBuffer& GetIndexBuffer() const;

	const Model* const GetModel() const;

	virtual void Update(unsigned int index);

	struct MVPUniformBuffer
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 projection;
		glm::mat4 pose[120];
		glm::mat4 invBindPose[120];
		bool isSkinned;
	};

	virtual const UniformBuffer* const GetUniformBuffer(unsigned int binding) const;

	virtual const Image* const GetImage(unsigned int binding) const;

protected:

	MVPUniformBuffer mvp;

	Model* const model;

	VertexBuffer* modelVertexBuffer;
	
	IndexBuffer* modelIndexBuffer;

	std::vector<Texture*> textures;

	std::vector<UniformBuffer*> uniformBuffers;

	DescriptorSet* descriptorSet;

	Pose* animatedPose;

	float playback;

private:

	void CreateTextures();

	void CreateUniformBuffers();

	void InitializeBuffers();

	void CreateDescriptorSets();

};

#endif // GRAPHICSOBJECT_H