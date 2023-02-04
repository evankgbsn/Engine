#include "Transform.h"

#include "Math.h"

using namespace Math;

Transform::Transform() :
	position(glm::vec3(0,0,0)),
	rotation(glm::quat(1,0,0,0)),
	scale(glm::vec3(1,1,1))
{
}

Transform::Transform(const glm::vec3& pos, const glm::quat& rot, const glm::vec3& s) :
	position(pos),
	rotation(rot),
	scale(s)
{
}

Transform::~Transform()
{
}

bool Math::Transform::operator!=(const Transform& other)
{
	return (position == other.position && rotation == other.rotation && scale == other.scale);
}

Transform Transform::Combine(const Transform& a, const Transform& b)
{
	Transform out;

	out.scale = a.scale * b.scale;
	out.rotation = a.rotation * b.rotation;

	out.position = a.rotation * (a.scale * b.position);
	out.position = a.position + out.position;

	return out;
}

Transform Transform::Inverse(const Transform& t)
{
	Transform inv;

	inv.rotation = glm::inverse(t.rotation);

	float epsilon = 0.000001f;

	inv.scale.x = fabs(t.scale.x) < epsilon ? 0.0f : 1.0f / t.scale.x;
	inv.scale.y = fabs(t.scale.y) < epsilon ? 0.0f : 1.0f / t.scale.y;
	inv.scale.z = fabs(t.scale.z) < epsilon ? 0.0f : 1.0f / t.scale.z;

	glm::vec3 invTrans = t.position * -1.0f;
	inv.position = inv.rotation * (inv.scale * invTrans);

	return inv;
}

Transform Transform::Mix(const Transform& a, const Transform& b, const float& time)
{
	glm::quat bRot = b.rotation;

	if (glm::dot(a.rotation, bRot) < 0.0f)
	{
		bRot = -bRot;
	}

	return Transform(
		Math::Lerp(a.position, b.position, time),
		Math::NLerp(a.rotation, bRot, time),
		Math::Lerp(a.scale, b.scale, time)
	);
}

Transform Math::Transform::FromMat4(const glm::mat4& m)
{
	glm::vec3 scale(glm::length(m[0]), glm::length(m[1]), glm::length(m[2]));

	glm::vec3 position(m[3]);
	
	glm::mat3 rotation(
		glm::normalize(m[0]),
		glm::normalize(m[1]),
		glm::normalize(m[2])
	);

	return Transform(position, rotation, scale);
}

glm::vec3 Math::Transform::TransformPoint(const Transform& t, const glm::vec3& p)
{
	glm::vec3 out;

	out = t.rotation * (t.scale * p);
	out = t.position * out;

	return out;
}

glm::vec3 Math::Transform::TransformVector(const Transform& t, const glm::vec3& v)
{
	glm::vec3 out;

	out = t.rotation * (t.scale * v);

	return out;
}

glm::mat4 Math::Transform::ToMat4() const
{
	glm::vec3 x = rotation * glm::vec3(1, 0, 0);
	glm::vec3 y = rotation * glm::vec3(0, 1, 0);
	glm::vec3 z = rotation * glm::vec3(0, 0, 1);

	x *= scale.x;
	y *= scale.y;
	z *= scale.z;

	return glm::mat4(
		x.x, x.y, x.z, 0,
		y.x, y.y, y.z, 0,
		z.x, z.y, z.z, 0,
		position.x, position.y, position.z, 1
	);
}

glm::vec3& Math::Transform::Position()
{
	return position;
}

glm::quat& Math::Transform::Rotation()
{
	return rotation;
}

glm::vec3& Math::Transform::Scale()
{
	return scale;
}
