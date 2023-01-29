#ifndef GRAPHICSOBJECTMANAGER_H
#define GRAPHICSOBJECTMANAGER_H

#include <vector>

class GraphicsObject;
class Model;
class DescriptorSetLayout;

class GraphicsObjectManager
{

public:

	static void Initialize();

	static void Terminate();

	static const DescriptorSetLayout* const GetDescriptorSetLayout();
	
	static const GraphicsObject* const GetDefault();

	static GraphicsObject* const CreateGraphicsObject(Model* const model);

	static const std::vector<GraphicsObject*>& GetGraphicsObjets();

private:

	GraphicsObjectManager();

	~GraphicsObjectManager();

	GraphicsObjectManager(const GraphicsObjectManager&) = delete;

	GraphicsObjectManager& operator=(const GraphicsObjectManager&) = delete;

	GraphicsObjectManager(GraphicsObjectManager&&) = delete;

	GraphicsObjectManager& operator=(GraphicsObjectManager&&) = delete;
	
	void CreateDescriptorPools();

	static GraphicsObjectManager* instance;

	std::vector<GraphicsObject*> graphicsObjects;
};

#endif // GRAPHICSOBJECTMANAGER_H