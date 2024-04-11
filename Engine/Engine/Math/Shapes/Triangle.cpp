#include "Triangle.h"

Triangle::~Triangle()
{
}

const glm::vec3& Triangle::GetPoint0() const
{
	return point0;
}

const glm::vec3& Triangle::GetPoint1() const
{
	return point1;
}

const glm::vec3& Triangle::GetPoint2() const
{
	return point2;
}
