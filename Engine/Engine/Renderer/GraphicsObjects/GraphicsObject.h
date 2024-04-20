#ifndef GRAPHICSOBJECT_H
#define GRAPHICSOBJECT_H

#include "GraphicsObjectTypes.h"

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <atomic>

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

	GraphicsObject(const Model* const model);

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

	ObjectTypes::GraphicsObjectType GetGraphicsObjectType() const;

	bool Loaded();


protected:

	void InitializeDescriptorSets();

	virtual void CreateTextures() = 0;

	virtual void CreateUniformBuffers() = 0;

	const Model* const model;

	VertexBuffer* modelVertexBuffer;
	
	IndexBuffer* modelIndexBuffer;

	std::vector<Texture*> textures;

	std::vector<UniformBuffer*> uniformBuffers;

	DescriptorSet* descriptorSet;

	std::string shaderName;

	ObjectTypes::GraphicsObjectType type;

	std::atomic<bool> loaded;

private:
	
	friend class GraphicsObjectManager;
	void Load();

	void CreateDescriptorSets();

	void InitializeBuffers();
};

#endif // GRAPHICSOBJECT_H