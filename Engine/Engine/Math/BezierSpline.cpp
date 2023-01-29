#include "BezierSpline.h"

#include "Math.h"

using namespace Math;

BezierSpline::BezierSpline(glm::vec3 point1, glm::vec3 point2, glm::vec3 control1, glm::vec3 control2) :
	p1(point1),
	p2(point2),
	c1(control1),
	c2(control2)
{
}

BezierSpline::~BezierSpline()
{
}

glm::vec3 BezierSpline::Interpolate(float t) const
{
	// This can be simplified and will be more efficent but this looks much cleaner and is easier to understand.

	glm::vec3 A = Lerp(p1, c1, t);
	glm::vec3 B = Lerp(c2, p2, t);
	glm::vec3 C = Lerp(c1, c2, t);

	glm::vec3 D = Lerp(A, C, t);
	glm::vec3 E = Lerp(C, B, t);

	return Lerp(D, E, t);
}
