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

	const DescriptorSet& GetDescriptorSet(unsigned int index) const;

	const UniformBuffer& GetMVPUniformBuffer(unsigned int index) const;

	const VertexBuffer& GetVertexBuffer() const;

	const IndexBuffer& GetIndexBuffer() const;

	const Model* const GetModel() const;

	virtual void Update(unsigned int index);

	void SlowUpdate(unsigned int index);

	struct MVPUniformBuffer
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 projection;
		glm::mat4 pose[120];
		glm::mat4 invBindPose[120];
		bool isSkinned;
	};

protected:

	MVPUniformBuffer mvp;

	Model* const model;

	VertexBuffer* modelVertexBuffer;
	
	IndexBuffer* modelIndexBuffer;

	Texture* texture;

	std::vector<UniformBuffer*> mvpUniformBuffers;

	std::vector<DescriptorSet*> descriptorSets;

private:

	void CreateUniformBuffers();

	void InitializeBuffers();

	void CreateDescriptorSets();

};

#endif // GRAPHICSOBJECT_H