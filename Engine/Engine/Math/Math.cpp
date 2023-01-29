#include "Math.h"

glm::vec3 Math::Lerp(const glm::vec3& p0, const glm::vec3& p1, float t)
{
	return p0 * (1.0f - t) + p1 * t;
};

glm::quat Math::NLerp(const glm::quat& q0, const glm::quat& q1, float t)
{
	return glm::normalize(q0 + (q1 - q0) * t);
}