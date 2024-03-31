#ifndef TRANSFORMSYSTEM_H
#define TRANSFORMSYSTEM_H

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

#endif //TRANSFORMSYSTEM_H