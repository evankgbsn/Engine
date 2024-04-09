#ifndef MATH_H
#define MATH_H

#include "../Renderer/Model/Model.h"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

class OrientedBoundingBox;
class AxisAlignedBoundingBox;
class BoundingSphere;
class PerfectCollisionVolume;
class CollisionVolume;

namespace Math
{
	glm::vec3 Lerp(const glm::vec3& p0, const glm::vec3& p1, float t);

	glm::quat NLerp(const glm::quat& q0, const glm::quat& q1, float t);

	bool PointInTriangle(const glm::vec2& pointToTest, const glm::vec2& trianglePoint0, const glm::vec2& trianglePoint1, const glm::vec2& trianglePoint2);

	void WorldSpacePointToWindowSpace(const glm::vec4& point, const glm::vec2& windowDimensions, const glm::vec2 offset, glm::vec2& outWindowSpacePoint);

	void ModelSpaceToWorldSpace(const glm::mat4& view, const glm::mat4& projection, const glm::mat4& model, const glm::vec4& point, glm::vec4& outWorldSpacePoint);

	bool PointIn2DModel(const Model* const model, const glm::mat4& view, const glm::mat4& projection, const glm::mat4& modelMat, const glm::vec2& point, const glm::vec2& windowDimensions, const glm::vec2& offset = glm::vec2(0.0f));

	float ChangeRange(float currentBegin, float currentEnd, float newBegin, float newEnd, float value);

	float ProjLength(const glm::vec3& v, const  glm::vec3& w);
};

#endif // MATH_H