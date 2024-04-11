#ifndef AXISALIGNEDBOUNDINGBOX_H
#define AXISALIGNEDBOUNDINGBOX_H

#include <glm/glm.hpp>

class AxisAlignedBoundingBox
{
public:

	AxisAlignedBoundingBox(const glm::vec3& initialOrigin, const glm::vec3& initialSize);

	~AxisAlignedBoundingBox();

	void FromMinAndMax(const glm::vec3& min, const glm::vec3& max);

	const glm::vec3& GetOrigin() const;

	const glm::vec3& GetSize() const;

	glm::vec3 GetMin() const;

	glm::vec3 GetMax() const;

private:

	AxisAlignedBoundingBox(const AxisAlignedBoundingBox&) = delete;

	AxisAlignedBoundingBox& operator=(const AxisAlignedBoundingBox&) = delete;
	
	AxisAlignedBoundingBox(AxisAlignedBoundingBox&&) = delete;

	AxisAlignedBoundingBox& operator=(AxisAlignedBoundingBox&&) = delete;

	glm::vec3 origin;

	glm::vec3 size;

};

#endif // AXISALIGNEDBOUNDINGBOX_H