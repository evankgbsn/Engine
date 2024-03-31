#ifndef GRAPHICSOBJECTMANAGER_H
#define GRAPHICSOBJECTMANAGER_H

#include "GraphicsObjectTypes.h"

#include <vector>
#include <list>
#include <unordered_map>
#include <string>
#include <vulkan/vulkan.h>
#include <mutex>
#include <functional>
#include <glm/glm.hpp>

class GraphicsObject;
class TexturedStaticGraphicsObject;
class TexturedStatic2DGraphicsObject;
class TexturedAnimatedGraphicsObject;
class LitTexturedStaticGraphicsObject;
class ColoredStaticGraphicsObject;
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

	static void CreateTexturedStaticGraphicsObject(Model* const model, Texture* const texture, std::function<void(TexturedStaticGraphicsObject*)> callback);

	static void CreateTexturedAnimatedGraphicsObject(Model* const model, Texture* const texture, std::function<void(TexturedAnimatedGraphicsObject*)> callback);

	static void CreateGoochGraphicsObject(Model* const model, Texture* const texture, std::function<void(GoochGraphicsObject*)> callback);

	static void CreateLitTexturedStaticGraphicsObject(Model* const model, Texture* const texture, std::function<void(LitTexturedStaticGraphicsObject*)> callback);

	static void CreateTexturedStatic2DGraphicsObject(Model* const model, Texture* const texture, std::function<void(TexturedStatic2DGraphicsObject*)> callback);

	static void CreateColoredStaticGraphicsObject(Model* const model, const glm::vec4& color, std::function<void(ColoredStaticGraphicsObject*)> callback);

	static void WireFrame(GraphicsObject* obj, ObjectTypes::GraphicsObjectType type);

	static void Solid(GraphicsObject* obj, ObjectTypes::GraphicsObjectType type);

	static const std::vector<GraphicsObject*>& GetTexturedStaticGraphicsObjets();

	static const std::vector<GraphicsObject*>& GetTexturedAnimatedGraphicsObjects();

	static void ToggleGraphicsObjectDraw(GraphicsObject* const graphicsObjectToToggle, ObjectTypes::GraphicsObjectType type);

	static void ExecutePendingCommands();

	static void UpdateObjects();

	static void DrawObjects(VkCommandBuffer& buffer, unsigned int imageIndex);

	static const ShaderPipelineStage* const GetShaderPipelineStage(const std::string& shaderName);

	static bool Operating();

	static void DeleteGraphicsObject(GraphicsObject* go);

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

	void CreateQueuedGraphicsObjects();

	void DeleteQueuedGraphicsObjects();

	void ToggleQueuedGraphicsObjects();

	void ConvertPipelineForQueuedGraphicsObjects();

	bool IsPipelineFromShader(const std::string& pipelineKey);

	static GraphicsObjectManager* instance;

	static bool shouldUpdate;

	std::mutex enqueueStaticMutex;

	std::mutex enqueueStatic2DMutex;

	std::mutex enqueueAnimatedMutex;

	std::mutex enqueueGoochMutex;

	std::mutex enqueueLitStaticMutex;

	std::mutex enqueueColoredStaticMutex;

	std::mutex updateMutex;

	std::vector<GraphicsObject*> texturedStaticGraphicsObjects;

	std::list<std::pair<GraphicsObject*, unsigned int>> disabledTexturedStaticGraphicsObjects;

	std::vector<GraphicsObject*> texturedStaticGraphicsObjectsWireFrame;

	std::list<std::pair<GraphicsObject*, unsigned int>> disabledTexturedStaticGraphicsObjectsWireFrame;

	std::vector<GraphicsObject*> animatedTexturedGraphicsObjects;

	std::list<std::pair<GraphicsObject*, unsigned int>> disabledAnimatedTexturedGraphicsObjects;

	std::vector<GraphicsObject*> animatedTexturedGraphicsObjectsWireFrame;

	std::list<std::pair<GraphicsObject*, unsigned int>> disabledAnimatedTexturedGraphicsObjectsWireFrame;

	std::vector<GraphicsObject*> goochGraphicsObjects;

	std::list<std::pair<GraphicsObject*, unsigned int>>  disabledGoochGraphicsObjects;

	std::vector<GraphicsObject*> goochGraphicsObjectsWireFrame;

	std::list<std::pair<GraphicsObject*, unsigned int>> disabledGoochGraphicsObjectsWireFrame;

	std::vector<GraphicsObject*> litTexturedStaticGraphicsObjects;

	std::list<std::pair<GraphicsObject*, unsigned int>> disabledLitTexturedStaticGraphicsObjects;

	std::vector<GraphicsObject*> litTexturedStaticGraphicsObjectsWireFrame;

	std::list<std::pair<GraphicsObject*, unsigned int>> disabledLitTexturedStaticGraphicsObjectsWireFrame;

	std::vector<GraphicsObject*> texturedStatic2DGraphicsObjects;

	std::list<std::pair<GraphicsObject*, unsigned int>> disabledTexturedStatic2DGraphicsObjects;

	std::vector<GraphicsObject*> texturedStatic2DGraphicsObjectsWireFrame;

	std::list<std::pair<GraphicsObject*, unsigned int>> disabledTexturedStatic2DGraphicsObjectsWireFrame;

	std::vector<GraphicsObject*> coloredStaticGraphicsObjects;

	std::list<std::pair<GraphicsObject*, unsigned int>> disabledColoredStaticGraphicsObjects;

	std::vector<GraphicsObject*> coloredStaticGraphicsObjectsWireFrame;

	std::list<std::pair<GraphicsObject*, unsigned int>> disabledColoredStaticGraphicsObjectsWireFrame;

	const Window& window;

	std::unordered_map<std::string, std::pair<ShaderPipelineStage*, GraphicsPipeline*>> graphicsPipelines;

	std::vector<Shader*> shaders;

	static const std::string shaderDirectoryName;

	std::list<std::function<void()>> graphicsObjectCreateQueue;

	std::list<std::function<void()>> graphicsObjectDeleteQueue;

	std::list<std::function<void()>> graphicsObjectPipelineConversionQueue;

	std::list<std::function<void()>> graphicsObjectToggleQueue;
};

#endif // GRAPHICSOBJECTMANAGER_H