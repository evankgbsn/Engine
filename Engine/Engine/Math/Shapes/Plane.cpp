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

bool Plane::PointOnPlane(const glm::vec3& point) const
{
    float planeEq = PlaneEquation(point);
    return planeEq >= -0.000005f && planeEq <= 0.000005f;
}

glm::vec3 Plane::ClosestPoint(const glm::vec3& point) const
{
    return point - normal * PlaneEquation(point);
}
