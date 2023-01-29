#ifndef STAGINGBUFFER_H
#define STAGINGBUFFER_H

#include "Buffer.h"
#include "../Model/Vertex.h"

#include <vector>

class StagingBuffer : public Buffer
{

public:

	StagingBuffer() = delete;

	StagingBuffer(unsigned int sizeInBytes);

	~StagingBuffer();

	StagingBuffer(const StagingBuffer&) = delete;

	StagingBuffer& operator=(const StagingBuffer&) = delete;

	StagingBuffer(StagingBuffer&&) = delete;

	StagingBuffer& operator=(StagingBuffer&&) = delete;

private:


};


#endif // STAGINGBUFFER_H