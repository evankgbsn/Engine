#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Math
{
	class Transform
	{

	public:

		Transform();

		Transform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale);

		~Transform();

		Transform(const Transform&) = default;

		Transform& operator=(const Transform&) = default;

		Transform(Transform&&) = default;

		Transform& operator=(Transform&&) = default;

		bool operator!=(const Transform&);

		static Transform Combine(const Transform& a, const Transform& b);

		static Transform Inverse(const Transform& t);

		static Transform Mix(const Transform& a, const Transform& b, const float& time);

		static Transform FromMat4(const glm::mat4& m);

		static glm::vec3 TransformPoint(const Transform& t, const glm::vec3& p);

		static glm::vec3 TransformVector(const Transform& t, const glm::vec3& v);

		glm::mat4 ToMat4() const;

		glm::vec3& Position();

		glm::quat& Rotation();

		glm::vec3& Scale();

	private:

		glm::vec3 position;

		glm::quat rotation;

		glm::vec3 scale;

	};
};

#endif // TRANSFORM_H