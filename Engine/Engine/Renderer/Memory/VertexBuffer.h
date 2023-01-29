#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

#include "Buffer.h"
#include "../Model/Vertex.h"

class StagingBuffer;

class VertexBuffer : public Buffer
{

public:

	VertexBuffer() = delete;

	VertexBuffer(unsigned int sizeInBytes);

	~VertexBuffer();

	VertexBuffer(const VertexBuffer&) = delete;

	VertexBuffer& operator=(const VertexBuffer&) = delete;

	VertexBuffer(VertexBuffer&&) = delete;

	VertexBuffer& operator=(VertexBuffer&&) = delete;

private:

};


#endif // VERTEX_BUFFER_H