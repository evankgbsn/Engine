#include "OrientedBoundingBox.h"

OrientedBoundingBox::OrientedBoundingBox(const glm::vec3& initialOrigin, const glm::vec3& initialSize, const glm::mat4& initialOrientation) :
    origin(initialOrigin),
    size(initialSize),
    orientation(initialOrientation)
{
}

OrientedBoundingBox::~OrientedBoundingBox()
{
}

const glm::vec3& OrientedBoundingBox::GetOrigin() const
{
    return origin;
}

const glm::vec3& OrientedBoundingBox::GetSize() const
{
    return size;
}

const glm::mat4& OrientedBoundingBox::GetOrientation() const
{
    return orientation;
}
