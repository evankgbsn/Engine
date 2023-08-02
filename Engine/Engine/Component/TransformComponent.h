#ifndef TransformComponent_H
#define TransformComponent_H

#include "Component.h"

class TransformComponent : public Component
{
public:

	TransformComponent();

	~TransformComponent();

	TransformComponent(const TransformComponent&) = default;

	TransformComponent& operator=(const TransformComponent&) = default;

	TransformComponent(TransformComponent&&) = default;

	TransformComponent& operator=(TransformComponent&&) = default;

protected:

private:

};


#endif //TransformComponent_H