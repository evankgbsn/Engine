#include "HermiteSpline.h"

using namespace Math;

glm::vec3 HermiteSpline::Interpolate(float t)
{
	return 
		p0 * ((1.0f + 2.0f * t) * ((1.0f - t) * (1.0f - t))) +
		t0 * (t * ((1.0f - t) * (1.0f - t))) +
		p1 * ((t * t) * (3.0f -2.0f * t)) + 
		t1 * ((t * t) * (t - 1.0f));
}

HermiteSpline::HermiteSpline(glm::vec3 point1, glm::vec3 point2, glm::vec3 tangent1, glm::vec3 tangent2) :
	p0(point1),
	p1(point2),
	t0(tangent1),
	t1(tangent2)
{
	
}

HermiteSpline::~HermiteSpline()
{
}
