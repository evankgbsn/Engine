#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include "../Renderer.h"

class MemoryManager
{
public:

	static void Initialize();

	static void Terminate();

	static const VmaAllocator& GetAllocator();

private:

	void InitializeVMAAllocator();

	void TerminateVMAAllocator();

	MemoryManager();

	~MemoryManager();

	MemoryManager(const MemoryManager&) = delete;

	MemoryManager& operator=(const MemoryManager&) = delete;

	MemoryManager(MemoryManager&&) = delete;

	MemoryManager& operator=(MemoryManager&&) = delete;

	static MemoryManager* instance;

	VmaAllocator allocator;

};

#endif // MEMORYMANAGER_H