#ifndef Component_H
#define Component_H

class Component
{
public:

	Component();

	~Component();

private:

	Component(const Component&) = delete;

	Component& operator=(const Component&) = delete;

	Component(Component&&) = delete;

	Component& operator=(component&&) = delete;
}

#endif // Component_H