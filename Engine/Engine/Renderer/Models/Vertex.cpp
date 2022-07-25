#include "Vertex.h"

Vertex::Vertex() :
	position(glm::vec2(0.0f, 0.0f)),
	color(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f))
{
}

Vertex::Vertex(const glm::vec2& p, const glm::vec4& c) :
	position(p),
	color(c)
{
}

Vertex::Vertex(glm::vec2&& p, glm::vec4&& c) :
	position(p),
	color(c)
{
}

Vertex::Vertex(const glm::vec2& p, glm::vec4&& c) :
	position(p),
	color(c)
{
}

Vertex::Vertex(glm::vec2&& p, const glm::vec4& c) :
	position(p),
	color(c)
{
}

Vertex::~Vertex()
{
}

Vertex::Vertex(const Vertex& vert) :
	position(vert.position),
	color(vert.color)
{
}

Vertex& Vertex::operator=(const Vertex& vert)
{
	position = vert.position;
	color = vert.color;
	return *this;
}

Vertex::Vertex(Vertex&& vert) noexcept :
	position(std::move(vert.position)),
	color(std::move(vert.color))
{
}

Vertex& Vertex::operator=(Vertex&& vert) noexcept
{
	position = std::move(vert.position);
	color = std::move(vert.color);
	return *this;
}
