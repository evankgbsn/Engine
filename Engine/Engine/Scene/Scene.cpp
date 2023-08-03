#include "Scene.h"

Scene::Scene() :
	entities(std::unordered_map<std::string, Entity*>()),
	systems(std::unordered_map<std::string, System*>())
{
}

Scene::~Scene()
{
}

void Scene::AddEntity(const std::string& name, Entity* const ent)
{
	entities.insert(std::make_pair(name, ent));
}

void Scene::AddSystem(const std::string& name, System* const sys)
{
	systems.insert(std::make_pair(name, sys));
}
