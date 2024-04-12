#ifndef RAY_H
#define RAY_H

#include <glm/glm.hpp>

class Ray
{
public:

	Ray(const glm::vec3& initialOrigin, const glm::vec3& initialDirection);

	~Ray();

	const glm::vec3& GetOrigin() const;

	const glm::vec3& GetDirection() const;

	bool PointIntersect(const glm::vec3& point) const;

	glm::vec3 ClosestPoint(const glm::vec3& point) const;

private:

	Ray(const Ray&) = delete;

	Ray& operator=(const Ray&) = delete;
	
	Ray(Ray&&) = delete;

	Ray& operator=(Ray&&) = delete;

	glm::vec3 origin;

	glm::vec3 direction;

};

#endif // RAY_H