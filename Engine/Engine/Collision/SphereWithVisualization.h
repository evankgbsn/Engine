#ifndef SPHEREWITHVISUALIZATION_H
#define SPHEREWITHVISUALIZATION_H

#include "../Math/Shapes/Sphere.h"

class GraphicsObject;
class ColoredStaticGraphicsObject;

class SphereWithVisualization : public Sphere
{
public:

	SphereWithVisualization(GraphicsObject* go);

	~SphereWithVisualization();

	void Update(const glm::mat4& transform);

private:

	SphereWithVisualization() = delete;

	SphereWithVisualization(const SphereWithVisualization&) = delete;

	SphereWithVisualization& operator=(const SphereWithVisualization&) = delete;

	SphereWithVisualization(SphereWithVisualization&&) = delete;

	SphereWithVisualization& operator=(SphereWithVisualization&&) = delete;

	GraphicsObject* wrapedGraphics;

	ColoredStaticGraphicsObject* graphics;

};

#endif // SPHEREWITHVISUALIZATION_H