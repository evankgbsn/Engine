#ifndef TEXTUREDANIMATEDGRAPHICSOBJECT_H
#define TEXTUREDANIMATEDGRAPHICSOBJECT_H

#include "GraphicsObject.h"

class Pose;
class Clip;
class Armature;
class Model;
class Texture;

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

protected:

	struct MVPUniformBuffer
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 projection;
		glm::mat4 pose[120];
		glm::mat4 invBindPose[120];
	};

	struct AnimationInstance
	{
		AnimationInstance(Clip* c, const Pose& p);
		~AnimationInstance();
		AnimationInstance(const AnimationInstance&) = default;
		AnimationInstance& operator=(const AnimationInstance&) = default;
		AnimationInstance(AnimationInstance&&) = default;
		AnimationInstance& operator=(AnimationInstance&&) = default;

		float speed;
		float playback;
		Pose* animatedPose;
		Clip* clip;

		void Update(Armature* const armature, MVPUniformBuffer& ubo);
	};

	virtual void CreateTextures() override;

	virtual void CreateUniformBuffers() override;

	virtual void Update() override;

	MVPUniformBuffer mvp;

	AnimationInstance animationInstance;

	Texture* texture;

private:

};


#endif // TEXTUREDANIMATEDGRAPHICSOBJECT_H