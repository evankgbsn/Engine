#ifndef Component_H
#define Component_H

#include <mutex>

/**
	Components do not manage themselves.
*/
class Component
{
public:

	enum class Type
	{
		VOID
	};

	Component();

	~Component();

	Type GetType() const;

protected:

	Component(const Component&) = default;

	Component& operator=(const Component&) = default;

	Component(Component&&) = default;

	Component& operator=(Component&&) = default;

	static std::mutex componentIdIterativeMutex;
	
	static unsigned long componentIdIterative;

	unsigned long componentId;

private:
	
	Type componentType;
};

#endif