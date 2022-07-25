#ifndef VERTEX_H
#define VERTEX_H

#include <glm/glm.hpp>

class Vertex
{

public:

	Vertex();

	Vertex(const glm::vec2& position, const glm::vec4& color);

	Vertex(glm::vec2&& position, glm::vec4&& color);

	Vertex(const glm::vec2& position, glm::vec4&& color);

	Vertex(glm::vec2&& position, const glm::vec4& color);

	~Vertex();

	Vertex(const Vertex&);

	Vertex& operator=(const Vertex&);

	Vertex(Vertex&&) noexcept;

	Vertex& operator=(Vertex&&) noexcept;

	glm::vec2 position;

	glm::vec4 color;

private:

};

#endif // VERTEX_H