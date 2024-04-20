#ifndef ORIENTEDBOUNDINGBOXWITHVISUALIZATION_H
#define ORIENTEDBOUNDINGBOXWITHVISUALIZATION_H

#include "../Math/Shapes/OrientedBoundingBox.h"

class ColoredStaticGraphicsObject;

class OrientedBoundingBoxWithVisualization : public OrientedBoundingBox
{
public:

	OrientedBoundingBoxWithVisualization(const glm::vec3& initialOrigin, const glm::vec3& initialSize, const glm::mat4& initialOrientation = glm::mat4(1.0f));

	OrientedBoundingBoxWithVisualization(const std::vector<Vertex>& vertices, const glm::mat4& initialOrientation = glm::mat4(1.0f));

	~OrientedBoundingBoxWithVisualization();

	void Update(const glm::mat4& transform);

	void ToggleVisibility();

	void SetColor(const glm::vec4& newColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

private:

	void CreateGraphics();

	ColoredStaticGraphicsObject* graphics;

};

#endif // ORIENTEDBOUNDINGBOXWITHVISUALIZATION_H