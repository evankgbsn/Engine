#ifndef BEZIERSPLINE_H
#define BEZIERSPLINE_H

#include <glm/glm.hpp>

namespace Math
{
	class BezierSpline
	{

	public:

		BezierSpline() = delete;

		BezierSpline(glm::vec3 p1, glm::vec3 p2, glm::vec3 c1, glm::vec3 c2);

		~BezierSpline();

		BezierSpline(const BezierSpline&) = delete;

		BezierSpline& operator=(const BezierSpline&) = delete;

		BezierSpline(BezierSpline&&) = delete;

		BezierSpline& operator=(BezierSpline&&) = delete;

		glm::vec3 Interpolate(float t) const;

	private:

		glm::vec3 p1;

		glm::vec3 p2;

		glm::vec3 c1;

		glm::vec3 c2;
	};
};

#endif // BEZIERSPLINE_H