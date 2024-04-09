#ifndef COLOREDANIMATEDGRAPHICSOBJECT_H
#define COLOREDANIMATEDGRAPHICSOBJECT_H

#include "GraphicsObject.h"
#include "3DTransformable.h"

class Model;
class Animation;
class DirectionalLight;

class ColoredAnimatedGraphicsObject : public GraphicsObject, public Graphics3DTransformable
{
public:

	ColoredAnimatedGraphicsObject(const Model* const model, const glm::vec4& color);

	~ColoredAnimatedGraphicsObject();

	void Update() override;

	void SetAnimationSpeed(float animationSpeed);

	void SetClip(unsigned int clipIndex);

	const glm::mat4* const GetAnimPoseArray();

	const glm::mat4* const GetAnimInvBindPoseArray();

	void SetColor(const glm::vec4& newColor);

	const glm::vec4& GetColor() const;

private:

	void CreateTextures() override;

	void CreateUniformBuffers() override;

	ColoredAnimatedGraphicsObject(const ColoredAnimatedGraphicsObject&) = delete;

	ColoredAnimatedGraphicsObject& operator=(const ColoredAnimatedGraphicsObject&) = delete;

	ColoredAnimatedGraphicsObject(ColoredAnimatedGraphicsObject&&) = delete;

	ColoredAnimatedGraphicsObject& operator=(ColoredAnimatedGraphicsObject&&) = delete;

	struct MVPUniformBuffer
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 projection;
	};

	struct AnimUniformBuffer
	{
		glm::mat4 pose[120];
		glm::mat4 invBindPose[120];
	};

	struct LightUniformBuffer
	{
		glm::vec4 ambient;
		glm::vec4 direction;
		glm::vec4 color;
	};

	struct ColorUBO
	{
		glm::vec4 color;
	};

	ColorUBO color;

	MVPUniformBuffer mvp;

	AnimUniformBuffer anim;

	LightUniformBuffer light;

	Animation* animation;

	DirectionalLight* dirLight;

};
#endif // COLOREDANIMATEDGRAPHICSOBJECT_H