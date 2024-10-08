#ifndef TEXTUREDANIMATEDGRAPHICSOBJECT_H
#define TEXTUREDANIMATEDGRAPHICSOBJECT_H

#include "GraphicsObject.h"
#include "3DTransformable.h"

class Pose;
class Clip;
class Armature;
class Model;
class Texture;
class Animation;
class Camera;
class DirectionalLight;

class TexturedAnimatedGraphicsObject : public GraphicsObject, public Graphics3DTransformable
{
public:

	TexturedAnimatedGraphicsObject() = delete;

	TexturedAnimatedGraphicsObject(const Model* const model, Texture* const texture);

	~TexturedAnimatedGraphicsObject();

	TexturedAnimatedGraphicsObject(const TexturedAnimatedGraphicsObject&) = delete;

	TexturedAnimatedGraphicsObject& operator=(const TexturedAnimatedGraphicsObject&) = delete;

	TexturedAnimatedGraphicsObject(TexturedAnimatedGraphicsObject&&) = delete;

	TexturedAnimatedGraphicsObject& operator=(TexturedAnimatedGraphicsObject&&) = delete;

	bool ToggleLoopAnimation();

	void SetAnimationSpeed(float animationSpeed);

	void SetClip(unsigned int clipIndex);

	unsigned int GetClip() const;

	float GetAnimationSpeed() const;

	const glm::mat4* const GetAnimPoseArray();

	const glm::mat4* const GetAnimInvBindPoseArray();

protected:

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

	virtual void CreateTextures() override;

	virtual void CreateUniformBuffers() override;

	virtual void Update() override;

	MVPUniformBuffer mvp;

	AnimUniformBuffer anim;

	LightUniformBuffer light;

	Animation* animation;

	Texture* texture;

	DirectionalLight* dirLight;

	unsigned int clip;

private:

};

#endif // TEXTUREDANIMATEDGRAPHICSOBJECT_H