#ifndef PLAYER_H
#define PLAYER_H

#include "Entity/GameObject.h"

#include<functional>

class TexturedAnimatedGraphicsObject;
class ColoredStaticGraphicsObject;
class BoundingSphere;
class AxisAlignedBoundingBox;
class Model;
class Texture;

class Player : public GameObject
{

public:

	Player();

	~Player();

	void Update() override;

private:

	Player(const Player&) = delete;

	Player& operator=(const Player&) = delete;

	Player(Player&&) = delete;

	Player operator=(Player&&) = delete;

	void RegisterInput();

	TexturedAnimatedGraphicsObject* graphics;

	ColoredStaticGraphicsObject* testObj;

	BoundingSphere* collisionVolume;

	BoundingSphere* randomCollisionSphere;

	AxisAlignedBoundingBox* randomCollisionBox;

	Model* model;

	Texture* texture;

	float moveSpeed;

	bool movementInProgress;

	unsigned int direction = 5;

	std::function<void(int)>* wPress;

	std::function<void(int)>* aPress;

	std::function<void(int)>* sPress;

	std::function<void(int)>* dPress;

	std::function<void(int)>* wPressed;

	std::function<void(int)>* aPressed;

	std::function<void(int)>* sPressed;

	std::function<void(int)>* dPressed;

	std::function<void(int)>* setClipOnMoveRelease;

	std::function<void(int)>* setClipOnJumpPress;   

};

#endif // PLAYER_H