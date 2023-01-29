#ifndef INDEXBUFFER_H
#define INDEXBUFFER_H

#include "Buffer.h"

class IndexBuffer : public Buffer
{

public:
	
	IndexBuffer() = delete;

	IndexBuffer(unsigned int sizeInBytes);

	~IndexBuffer();

	IndexBuffer(const IndexBuffer&) = delete;

	IndexBuffer& operator=(const IndexBuffer&) = delete;

	IndexBuffer(IndexBuffer&&) = delete;

	IndexBuffer& operator=(IndexBuffer&&) = delete;

private:

};

#endif // INDEXBUFFER_H