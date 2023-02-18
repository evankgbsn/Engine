#ifndef GRAPHICSOBJECTMANAGER_H
#define GRAPHICSOBJECTMANAGER_H

#include <vector>
#include <unordered_map>
#include <string>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

class GraphicsObject;
class TexturedStaticGraphicsObject;
class TexturedAnimatedGraphicsObject;
class GoochGraphicsObject;
class Model;
class DescriptorSetLayout;
class GraphicsPipeline;
class ShaderPipelineStage;
class Window;
class Shader;
class Texture;

class GraphicsObjectManager
{

public:

	static void Initialize(const Window& window);

	static void Terminate();

	static TexturedStaticGraphicsObject* const CreateTexturedStaticGraphicsObject(Model* const model, Texture* const texture);

	static TexturedAnimatedGraphicsObject* const CreateTexturedAnimatedGraphicsObject(Model* const model, Texture* const texture);

	static GoochGraphicsObject* const CreateGoochGraphicsObject(Model* const model, Texture* const texture);

	static const std::vector<GraphicsObject*>& GetTexturedStaticGraphicsObjets();

	static const std::vector<GraphicsObject*>& GetTexturedAnimatedGraphicsObjects();

	static void DrawObjects(VkCommandBuffer& buffer, unsigned int imageIndex);

	static const ShaderPipelineStage* const GetShaderPipelineStage(const std::string& shaderName);

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

	std::vector<GraphicsObject*> staticGraphicsObjects;

	std::vector<GraphicsObject*> animatedGraphicsObjects;

	std::vector<GraphicsObject*> goochGraphicsObjects;

	const Window& window;

	std::unordered_map<std::string, std::pair<ShaderPipelineStage*, GraphicsPipeline*>> graphicsPipelines;

	std::vector<Shader*> shaders;

	static const std::string shaderDirectoryName;
};

#endif // GRAPHICSOBJECTMANAGER_H