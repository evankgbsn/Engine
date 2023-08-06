#ifndef GRAPHICSOBJECT_H
#define GRAPHICSOBJECT_H

#include <glm/glm.hpp>
#include <vector>
#include <string>

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

	virtual ~GraphicsObject();

	GraphicsObject(const GraphicsObject&) = delete;

	GraphicsObject& operator=(const GraphicsObject&) = delete;

	GraphicsObject(GraphicsObject&&) = delete;

	GraphicsObject& operator=(GraphicsObject&&) = delete;

	const DescriptorSet& GetDescriptorSet() const;

	const VertexBuffer& GetVertexBuffer() const;

	const IndexBuffer& GetIndexBuffer() const;

	const Model* const GetModel() const;

	virtual void Update() = 0;

	virtual const UniformBuffer* const GetUniformBuffer(unsigned int binding) const;

	virtual const Image* const GetImage(unsigned int binding) const;

protected:

	void InitializeDescriptorSets();

	virtual void CreateTextures() = 0;

	virtual void CreateUniformBuffers() = 0;

	Model* const model;

	VertexBuffer* modelVertexBuffer;
	
	IndexBuffer* modelIndexBuffer;

	std::vector<Texture*> textures;

	std::vector<UniformBuffer*> uniformBuffers;

	DescriptorSet* descriptorSet;

	std::string shaderName;

private:
	
	void CreateDescriptorSets();

	void InitializeBuffers();
};

#endif // GRAPHICSOBJECT_H