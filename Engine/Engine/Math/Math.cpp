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

float Math::ProjLength(const glm::vec3& v, const glm::vec3& w)
{
	return glm::dot(v, w) / glm::length(w);
}