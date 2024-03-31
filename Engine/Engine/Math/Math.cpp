#include "Math.h"

#include "glm/gtc/matrix_access.hpp"
#include "glm/gtx/projection.hpp"


glm::vec3 Math::Lerp(const glm::vec3& p0, const glm::vec3& p1, float t)
{
	return p0 * (1.0f - t) + p1 * t;
};

glm::quat Math::NLerp(const glm::quat& q0, const glm::quat& q1, float t)
{
	return glm::normalize(q0 + (q1 - q0) * t);
}

bool Math::PointInTriangle(const glm::vec2& pointToTest, const glm::vec2& trianglePoint0, const glm::vec2& trianglePoint1, const glm::vec2& trianglePoint2)
{
	// https://stackoverflow.com/questions/2049582/how-to-determine-if-a-point-is-in-a-2d-triangle
	auto sign = [](const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3) -> float
	{
		return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
	};

	auto pointInTriangle = [sign](const glm::vec2& point, const glm::vec2& a, const glm::vec2& b, const glm::vec2& c) -> bool
	{
		float d1, d2, d3;
		bool has_neg, has_pos;

		d1 = sign(point, a, b);
		d2 = sign(point, b, c);
		d3 = sign(point, c, a);

		has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
		has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

		return !(has_neg && has_pos);
	};

	return pointInTriangle(pointToTest, trianglePoint0, trianglePoint1, trianglePoint2);
}

void Math::WorldSpacePointToWindowSpace(const glm::vec4& point, const glm::vec2& windowDimensions, const glm::vec2 offset, glm::vec2& outWindowSpacePoint)
{
	glm::vec2 windowSizeAndOffset = windowDimensions + offset;

	double vert[2] =
	{
		(((point.x / point.w) + 1.0) / 2.0) * windowSizeAndOffset.x,
		(((point.y / point.w) + 1.0) / 2.0) * windowSizeAndOffset.y
	};

	outWindowSpacePoint = glm::vec2(vert[0], vert[1]);
}

void Math::ModelSpaceToWorldSpace(const glm::mat4& view, const glm::mat4& projection, const glm::mat4& model, const glm::vec4& point, glm::vec4& outWorldSpacePoint)
{
	outWorldSpacePoint = projection * view * model * point;
}

bool Math::PointIn2DModel(const Model* const model, const glm::mat4& view, const glm::mat4& projection, const glm::mat4& modelMat, const glm::vec2& point, const glm::vec2& windowDimensions, const glm::vec2& offset)
{
	const std::vector<Vertex>& vertices = model->GetVertices();

	static glm::vec4 triangleVerts[3] = { glm::vec4(0.0f), glm::vec4(0.0f), glm::vec4(0.0f) };

	for (unsigned int i = 0; i < model->GetIndices().size(); i += 3)
	{
		Math::ModelSpaceToWorldSpace(view, projection, modelMat, glm::vec4(vertices[model->GetIndices()[i]].GetPosition(), 1.0f), triangleVerts[0]);
		Math::ModelSpaceToWorldSpace(view, projection, modelMat, glm::vec4(vertices[model->GetIndices()[i + 1]].GetPosition(), 1.0f), triangleVerts[1]);
		Math::ModelSpaceToWorldSpace(view, projection, modelMat, glm::vec4(vertices[model->GetIndices()[i + 2]].GetPosition(), 1.0f), triangleVerts[2]);

		static glm::vec2 windowSpacePos0;
		static glm::vec2 windowSpacePos1;
		static glm::vec2 windowSpacePos2;

		Math::WorldSpacePointToWindowSpace(triangleVerts[0], windowDimensions, glm::vec2(0.0f, 0.0f), windowSpacePos0);
		Math::WorldSpacePointToWindowSpace(triangleVerts[1], windowDimensions, glm::vec2(0.0f, 0.0f), windowSpacePos1);
		Math::WorldSpacePointToWindowSpace(triangleVerts[2], windowDimensions, glm::vec2(0.0f, 0.0f), windowSpacePos2);

		if (Math::PointInTriangle(point, windowSpacePos0, windowSpacePos1, windowSpacePos2))
		{
			return true;
		}
	}

	return false;
}

float Math::ChangeRange(float currentBegin, float currentEnd, float newBegin, float newEnd, float value)
{
	float oldRange = (currentEnd - currentBegin);
	float newRange = (newEnd - newBegin);

	return (((value - currentBegin) * newRange) / oldRange) + newBegin;
}

bool Math::Intersect(const BoundingSphere& sphere1, const BoundingSphere& sphere2)
{
	float radius1 = sphere1.GetRadius();
	float radius2 = sphere2.GetRadius();

	glm::vec3 center1 = sphere1.GetCenter();
	glm::vec3 center2 = sphere2.GetCenter();

	float length = glm::length(center1 - center2);

	return (length < (radius1 + radius2));
}

bool Math::Intersect(const BoundingSphere& sphere, const AxisAlignedBoundingBox& AABB)
{
	return  glm::length((glm::vec3(sphere.GetCenter()) - glm::clamp(glm::vec3(sphere.GetCenter()), AABB.GetMin(), AABB.GetMax()))) <= sphere.GetRadius();
}

bool Math::Intersect(const AxisAlignedBoundingBox& AABB1, const AxisAlignedBoundingBox& AABB2)
{
	return
		(AABB1.GetMax().x > AABB2.GetMin().x) &&
		(AABB2.GetMax().x > AABB1.GetMin().x) &&
		(AABB1.GetMax().y > AABB2.GetMin().y) &&
		(AABB2.GetMax().y > AABB1.GetMin().y) &&
		(AABB1.GetMax().z > AABB2.GetMin().z) &&
		(AABB2.GetMax().z > AABB1.GetMin().z);
}

bool Math::Intersect(const CollisionVolume& volume1, const CollisionVolume& volume2)
{
	return volume1.Intersect(volume2);
}

bool Math::Intersect(const OrientedBoundingBox& box1, const OrientedBoundingBox& box2)
{

	// Grab the uniform scale factor.
	float box1Scale = glm::length(glm::column(box1.GetWorld(), 0));
	float box2Scale = glm::length(glm::column(box2.GetWorld(), 0));

	// The normals of both obbs
	glm::vec3 normals[] = {

		glm::normalize(glm::column(box1.GetWorld(), 0)),
		glm::normalize(glm::column(box1.GetWorld(), 1)),
		glm::normalize(glm::column(box1.GetWorld(), 2)),
		glm::normalize(glm::column(box2.GetWorld(), 0)),
		glm::normalize(glm::column(box2.GetWorld(), 1)),
		glm::normalize(glm::column(box2.GetWorld(), 2))

	};

	unsigned int count = 0;

	// test on seperating axes.
	for (int x = 0; x < 6; x++)
	{
		for (int y = x; ++y < 6;)
		{
			glm::vec3 seperatingAxis = glm::cross(normals[x], normals[y]);

			if (glm::length(seperatingAxis) > FLT_EPSILON)
			{
				float centerToCenterProjLength = glm::length(glm::proj((box1.GetCenter() - box2.GetCenter()), seperatingAxis));


				glm::vec3 seperatingAxisInObb1 = glm::inverse(box1.GetWorld()) * glm::vec4(seperatingAxis, 0);
				float maxProjObb1 = abs(seperatingAxisInObb1.x * box1.GetMax().x) + abs(seperatingAxisInObb1.y * box1.GetMax().y) + abs(seperatingAxisInObb1.z * box1.GetMax().z);
				maxProjObb1 /= glm::length(seperatingAxis);
				maxProjObb1 *= box1Scale * box1Scale;

				glm::vec3 seperatingAxisInObb2 = glm::inverse(box2.GetWorld()) * glm::vec4(seperatingAxis, 0);
				float maxProjObb2 = abs(seperatingAxisInObb2.x * box2.GetMax().x) + abs(seperatingAxisInObb2.y * box2.GetMax().y) + abs(seperatingAxisInObb2.z * box2.GetMax().z);
				maxProjObb2 /= glm::length(seperatingAxis);
				maxProjObb2 *= box2Scale * box2Scale;

				if (centerToCenterProjLength >= maxProjObb1 + maxProjObb2)
				{
					return false;
				}
			}
			count++;
		}
	}

	return true;

}

bool Math::Intersect(const OrientedBoundingBox& box1, const AxisAlignedBoundingBox& box2)
{
	// Grab the uniform scale factor.
	float box1Scale = glm::length(glm::column(box1.GetWorld(), 0));
	float box2Scale = glm::length(glm::column(box2.GetWorld(), 0));

	glm::vec3 box2LocalMax = glm::inverse(box2.GetWorld()) * glm::vec4(box2.GetMax(), 1.0f);
	glm::vec3 box2LocalMin = glm::inverse(box2.GetWorld()) * glm::vec4(box2.GetMin(), 1.0f);

	// The normals of both obbs
	glm::vec3 normals[] = {

		glm::normalize(glm::column(box1.GetWorld(), 0)),
		glm::normalize(glm::column(box1.GetWorld(), 1)),
		glm::normalize(glm::column(box1.GetWorld(), 2)),
		glm::normalize(glm::column(box2.GetWorld(), 0)),
		glm::normalize(glm::column(box2.GetWorld(), 1)),
		glm::normalize(glm::column(box2.GetWorld(), 2))

	};

	unsigned int count = 0;

	// test on seperating axis.
	for (int x = 0; x < 6; x++)
	{
		for (int y = x; ++y < 6;)
		{
			glm::vec3 seperatingAxis = glm::cross(normals[x], normals[y]);

			if (glm::length(seperatingAxis) * glm::length(seperatingAxis) > FLT_EPSILON)
			{
				float centerToCenterProjLength = glm::length(glm::proj((box1.GetCenter() - box2.GetCenter()), seperatingAxis));

				glm::vec3 seperatingAxisInObb1 = glm::inverse(box1.GetWorld()) * glm::vec4(seperatingAxis, 0);
				float maxProjObb1 = abs(seperatingAxisInObb1.x * box1.GetMax().x) + abs(seperatingAxisInObb1.y * box1.GetMax().y) + abs(seperatingAxisInObb1.z * box1.GetMax().z);
				maxProjObb1 /= glm::length(seperatingAxis);
				maxProjObb1 *= box1Scale * box1Scale;

				glm::vec3 seperatingAxisInObb2 = glm::inverse(box2.GetWorld()) * glm::vec4(seperatingAxis, 0);
				float maxProjObb2 = abs(seperatingAxisInObb2.x * box2LocalMax.x) + abs(seperatingAxisInObb2.y * box2LocalMax.y) + abs(seperatingAxisInObb2.z * box2LocalMax.z);
				maxProjObb2 /= glm::length(seperatingAxis);
				maxProjObb2 *= box2Scale * box2Scale;

				if (centerToCenterProjLength >= maxProjObb1 + maxProjObb2)
				{
					return false;
				}
			}
			count++;
		}
	}

	return true;
}

bool Math::Intersect(const OrientedBoundingBox& box, const BoundingSphere& sphere)
{
	glm::mat4 inverse = glm::inverse(box.GetWorld());
	glm::vec4 center = glm::vec4(glm::vec3(sphere.GetCenter()), 1.0f);

	glm::vec4 obbLocalSphereCenter = inverse * center;
	glm::vec4 obbLocalClampedSphereCenter = glm::clamp(obbLocalSphereCenter, glm::vec4(box.GetMin(), 1.0f), glm::vec4(box.GetMax(), 1.0f));
	glm::vec4 worldClampedSphereCenter = box.GetWorld() * obbLocalClampedSphereCenter;

	return  glm::length(center - worldClampedSphereCenter) <= sphere.GetRadius();
}