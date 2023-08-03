#ifndef Scene_H
#define Scene_H

#include <string>
#include <unordered_map>

class Entity;
class System;

class Scene
{
public:

	Scene();

	~Scene();

	void AddEntity(Entity* const ent);

	void AddSystem(System* const sys);

protected:

private:

	Scene(const Scene&) = delete;

	Scene& operator=(const Scene&) = delete;

	Scene(Scene&&) = delete;

	Scene& operator=(Scene&&) = delete;

	std::unordered_map<std::string, Entity*> entities;
	std::unordered_map<std::string, System*> systems;
};

#endif //Scene_H