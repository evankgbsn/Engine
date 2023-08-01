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

	static std::mutex componentIdIterativeMutex;
	
	static unsigned long componentIdIterative;

	unsigned long componentId;

private:
	
	Component(const Component&) = delete;

	Component& operator=(const Component&) = delete;

	Component(Component&&) = delete;

	Component& operator=(Component&&) = delete;

	Type componentType;
};

#endif