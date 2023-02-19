#ifndef TEXTUREDANIMATEDGRAPHICSOBJECT_H
#define TEXTUREDANIMATEDGRAPHICSOBJECT_H

#include "GraphicsObject.h"

class Pose;
class Clip;
class Armature;
class Model;
class Texture;
class Animation;

class TexturedAnimatedGraphicsObject : public GraphicsObject
{
public:

	TexturedAnimatedGraphicsObject() = delete;

	TexturedAnimatedGraphicsObject(Model* const model, Texture* const texture);

	~TexturedAnimatedGraphicsObject();

	TexturedAnimatedGraphicsObject(const TexturedAnimatedGraphicsObject&) = delete;

	TexturedAnimatedGraphicsObject& operator=(const TexturedAnimatedGraphicsObject&) = delete;

	TexturedAnimatedGraphicsObject(TexturedAnimatedGraphicsObject&&) = delete;

	TexturedAnimatedGraphicsObject& operator=(TexturedAnimatedGraphicsObject&&) = delete;

	bool ToggleLoopAnimation();

	void SetAnimationSpeed(float animationSpeed);

	void SetClip(unsigned int clipIndex);

protected:

	struct MVPUniformBuffer
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 projection;
		glm::mat4 pose[120];
		glm::mat4 invBindPose[120];
	};

	virtual void CreateTextures() override;

	virtual void CreateUniformBuffers() override;

	virtual void Update() override;

	MVPUniformBuffer mvp;

	Animation* animation;

	Texture* texture;

	float shouldUpdateAnim = 0;

private:

};


#endif // TEXTUREDANIMATEDGRAPHICSOBJECT_H