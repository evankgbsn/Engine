#ifndef Component_H
#define Component_H

class Component
{
public:

	Component() = default;

	~Component() = default;

private:

	Component(const Component&) = delete;

	Component& operator=(const Component&) = delete;

	Component(Component&&) = delete;

	Component& operator=(Component&&) = delete;
};

#endif