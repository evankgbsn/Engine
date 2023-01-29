#ifndef MATH_H
#define MATH_H

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Math
{

	glm::vec3 Lerp(const glm::vec3& p0, const glm::vec3& p1, float t);

	glm::quat NLerp(const glm::quat& q0, const glm::quat& q1, float t);

};

#endif // MATH_H