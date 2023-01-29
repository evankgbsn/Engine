#ifndef HERMITESPLINE_H
#define HERMITESPLINE_H

#include <glm/glm.hpp>

namespace Math
{
	class HermiteSpline
	{

	public:

		HermiteSpline() = delete;

		HermiteSpline(glm::vec3 point1, glm::vec3 point2, glm::vec3 tangent1, glm::vec3 tangent2);

		~HermiteSpline();

		HermiteSpline(const HermiteSpline&) = delete;

		HermiteSpline& operator=(const HermiteSpline&) = delete;

		HermiteSpline(HermiteSpline&&) = delete;

		HermiteSpline& operator=(HermiteSpline&&) = delete;

		glm::vec3 Interpolate(float t);

	private:

		glm::vec3 p0;

		glm::vec3 p1;

		glm::vec3 t0;

		glm::vec3 t1;

	};
};

#endif // HERMITESPLINE_H