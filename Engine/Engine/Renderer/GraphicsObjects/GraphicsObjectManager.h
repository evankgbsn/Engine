#ifndef GRAPHICSOBJECTMANAGER_H
#define GRAPHICSOBJECTMANAGER_H

#include <vector>
#include <unordered_map>
#include <string>
#include <vulkan/vulkan.h>

class GraphicsObject;
class Model;
class DescriptorSetLayout;
class GraphicsPipeline;
class ShaderPipelineStage;
class Window;
class Shader;

class GraphicsObjectManager
{

public:

	static void Initialize(const Window& window);

	static void Terminate();

	static const DescriptorSetLayout* const GetDescriptorSetLayout();
	
	static const GraphicsObject* const GetDefault();

	static GraphicsObject* const CreateGraphicsObject(Model* const model);

	static const std::vector<GraphicsObject*>& GetGraphicsObjets();

	static void DrawObjects(VkCommandBuffer& buffer, unsigned int imageIndex);

private:

	GraphicsObjectManager() = delete;

	GraphicsObjectManager(const Window& window);

	~GraphicsObjectManager();

	GraphicsObjectManager(const GraphicsObjectManager&) = delete;

	GraphicsObjectManager& operator=(const GraphicsObjectManager&) = delete;

	GraphicsObjectManager(GraphicsObjectManager&&) = delete;

	GraphicsObjectManager& operator=(GraphicsObjectManager&&) = delete;
	
	void CreateDescriptorPools();

	void CreateGraphicsPipelines();

	void LoadShaders();

	static GraphicsObjectManager* instance;

	std::vector<GraphicsObject*> graphicsObjects;

	const Window& window;

	std::unordered_map<std::string, std::pair<ShaderPipelineStage*, GraphicsPipeline*>> graphicsPipelines;

	std::vector<Shader*> shaders;

	static const std::string shaderDirectoryName;
};

#endif // GRAPHICSOBJECTMANAGER_H