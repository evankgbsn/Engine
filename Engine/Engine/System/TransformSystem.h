#ifndef TransformSystem_H
#define TransformSystem_H

#include "System.h"

class TransformComponent;

class TransformSystem : public System
{
public:

	TransformSystem();

	~TransformSystem();

	TransformComponent* const CreateComponent();

protected:

private:

	std::vector<TransformComponent> transformComponents;
};

#endif //TransformSystem_H