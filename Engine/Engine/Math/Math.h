#ifndef MATH_H
#define MATH_H

#include "../Renderer/Model/Model.h"

#include "../Collision/AxisAlignedBoundingBox.h"
#include "../Collision/BoundingSphere.h"
#include "../Collision/OrientedBoundingBox.h"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Math
{
	glm::vec3 Lerp(const glm::vec3& p0, const glm::vec3& p1, float t);

	glm::quat NLerp(const glm::quat& q0, const glm::quat& q1, float t);

	bool PointInTriangle(const glm::vec2& pointToTest, const glm::vec2& trianglePoint0, const glm::vec2& trianglePoint1, const glm::vec2& trianglePoint2);

	void WorldSpacePointToWindowSpace(const glm::vec4& point, const glm::vec2& windowDimensions, const glm::vec2 offset, glm::vec2& outWindowSpacePoint);

	void ModelSpaceToWorldSpace(const glm::mat4& view, const glm::mat4& projection, const glm::mat4& model, const glm::vec4& point, glm::vec4& outWorldSpacePoint);

	bool PointIn2DModel(const Model* const model, const glm::mat4& view, const glm::mat4& projection, const glm::mat4& modelMat, const glm::vec2& point, const glm::vec2& windowDimensions, const glm::vec2& offset = glm::vec2(0.0f));

	float ChangeRange(float currentBegin, float currentEnd, float newBegin, float newEnd, float value);

	bool Intersect(const class BoundingSphere& sphere1, const class BoundingSphere& sphere2);

	bool Intersect(const class BoundingSphere& sphere, const class AxisAlignedBoundingBox& AABB);

	bool Intersect(const class AxisAlignedBoundingBox& AABB1, const class AxisAlignedBoundingBox& AABB2);

	bool Intersect(const class CollisionVolume& volume1, const class CollisionVolume& volume2);

	bool Intersect(const class OrientedBoundingBox& box1, const class OrientedBoundingBox& box2);

	bool Intersect(const class OrientedBoundingBox& box1, const class AxisAlignedBoundingBox& box2);

	bool Intersect(const class OrientedBoundingBox& box, const class BoundingSphere& sphere);
};

#endif // MATH_H