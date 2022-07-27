#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

#include "Buffer.h"

class VertexBuffer : public Buffer
{
public:

	VertexBuffer(uint32_t size);

	~VertexBuffer();

protected:

private:

	VertexBuffer() = delete;

	VertexBuffer(const VertexBuffer&) = delete;

	VertexBuffer& operator=(const VertexBuffer&) = delete;

	VertexBuffer(VertexBuffer&&) = delete;

	VertexBuffer& operator=(VertexBuffer&&) = delete;
};

#endif // VERTEX_BUFFER_H