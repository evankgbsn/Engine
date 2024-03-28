#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>
#include <glm/glm.hpp>

class BakedAnimation;

class Animation
{
public:

	Animation(const BakedAnimation& bakedAnimation);

	Animation() = delete;

	~Animation();

	Animation(const Animation&) = delete;

	Animation& operator=(const Animation&) = delete;

	Animation(Animation&&) = delete;

	Animation& operator=(Animation&&) = delete;

	void Update(glm::mat4* posePalette);

	void SetSpeed(float newSpeed);

private:

	float frameTimeAdvance;

	float playback;

	float speed;

	unsigned int index;

	const BakedAnimation& bakedAnimation;

};


#endif // ANIMATION_H