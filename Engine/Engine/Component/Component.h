#ifndef Component_H
#define Component_H

#include <mutex>
#include <functional>
#include <list>

/**
	Components do not manage themselves.
*/
class Component
{
public:

	enum class Type
	{
		VOID,
		TRANSFORM
	};

	Component(Type type = Type::VOID);

	virtual ~Component();

	Type GetType() const;

	void Operate();

protected:

	void QueueCommand(const std::function<void()>& command);

	Component(const Component&) = default;

	Component& operator=(const Component&) = default;

	Component(Component&&) = default;

	Component& operator=(Component&&) = default;

	static std::mutex componentIdIterativeMutex;
	
	static unsigned long componentIdIterative;

	unsigned long componentId;

	std::mutex commandQueueMutex;

	std::list<std::function<void()>> commands;

private:

	Component() = delete;
	
	Type componentType;
};

#endif