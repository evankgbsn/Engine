#ifndef GameObject_h
#define GameObject_h

#include "Entity.h"

class Component;
class GraphicsObject;

class GameObject : public Entity
{
public:

	GameObject();

	~GameObject();

	static GameObject* const Create(Component* optionalComponents[] = {}, unsigned int componentCount = 0);

	virtual void Update() override;

protected:

private:

	GameObject(Component* optionalComponents[], unsigned int componentCount);

	GameObject(const GameObject&) = delete;

	GameObject& operator=(const GameObject&) = delete;

	GameObject(GameObject&&) = delete;

	GameObject& operator=(GameObject&&) = delete;
};

#endif // GameObject_H