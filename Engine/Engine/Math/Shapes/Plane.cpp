#include "Plane.h"

Plane::Plane(const glm::vec3& initialNormal, float initialDistanceFromOrigin) :
    normal(initialNormal),
    distance(initialDistanceFromOrigin)
{
}

Plane::~Plane()
{
}

const glm::vec3& Plane::GetNormal() const
{
    return normal;
}

float Plane::GetDistance() const
{
    return distance;
}

float Plane::PlaneEquation(const glm::vec3& point) const
{
    return glm::dot(point, normal) - distance;
}
