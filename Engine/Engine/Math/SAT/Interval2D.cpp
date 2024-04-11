#include "Interval2D.h"

#include "../Shapes/Rectangle.h"
#include "../Shapes/OrientedRectangle.h"

Interval2D::Interval2D(const Rectangle& rectangle, const glm::vec2& axis)
{
	// Get the min and max points of the rectangle.
	const glm::vec2 minVert = rectangle.GetMin();
	const glm::vec2 maxVert = rectangle.GetMax();

	// Get all vertices of the rectangle.
	glm::vec2 verts[4] =
	{
		minVert, glm::vec2(minVert.x, maxVert.y),
		maxVert, glm::vec2(maxVert.x, minVert.y)
	};

	// Project each vertex onto the axis and store the largest and smallest values.
	min = max = glm::dot(axis, verts[0]);
	for (unsigned int i = 1; i < 4; ++i)
	{
		float projection = glm::dot(axis, verts[i]);
		if (projection < min)
		{
			min = projection;
		}
		if (projection > max)
		{
			max = projection;
		}
	}
}

Interval2D::Interval2D(const OrientedRectangle& rectangle, const glm::vec2& axis)
{
	// Construct a non-oriented version of the oriented rectangle.
	Rectangle localRectangle(rectangle.GetPosition() - rectangle.GetHalfExtents(), rectangle.GetHalfExtents() * 2.0f);

	// Get the min and max points of the non-oriented rectangle.
	const glm::vec2 minVert = localRectangle.GetMin();
	const glm::vec2 maxVert = localRectangle.GetMax();

	// Get all vertices of the non-oriented rectangle.
	glm::vec2 verts[4] =
	{
		minVert, glm::vec2(minVert.x, maxVert.y),
		maxVert, glm::vec2(maxVert.x, minVert.y)
	};

	// Create a rotation maxrix for the rotation of the oriented rectangle.
	glm::mat2 zRotation =
	{
		cosf(rectangle.GetRotation()), sinf(rectangle.GetRotation()),
		-sinf(rectangle.GetRotation()), cosf(rectangle.GetRotation())
	};

	// Rotate all vertices of the non-oriented rectangle.
	for (unsigned int i = 0; i < 4; ++i)
	{
		verts[i] = verts[i] - rectangle.GetPosition();
		verts[i] = verts[i] * zRotation + rectangle.GetPosition();
	}

	// Project each vertex onto the axis and store the largest and smallest values.
	min = max = glm::dot(axis, verts[0]);
	for (unsigned int i = 1; i < 4; ++i)
	{
		float projection = glm::dot(axis, verts[i]);
		if (projection < min)
		{
			min = projection;
		}
		if (projection > max)
		{
			max = projection;
		}
	}
}

Interval2D::~Interval2D()
{
}

float Interval2D::GetMin() const
{
	return min;
}

float Interval2D::GetMax() const
{
	return max;
}
