#ifndef PLAYER_H
#define PLAYER_H

#include "Entity/GameObject.h"

#include<functional>

class TexturedAnimatedGraphicsObject;
class BoundingSphere;
class OrientedBoundingBox;
class AxisAlignedBoundingBox;
class Model;
class Texture;
class PerfectAnimatedCollisionVolume;
class UserInterfaceItem;
class Circle;
class Rectangle;
class OrientedRectangle;

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

	Model* model;

	Texture* texture;

	TexturedAnimatedGraphicsObject* graphics;

	UserInterfaceItem* collisionObj2D;

	OrientedRectangle* collider2D;

	UserInterfaceItem* collisionObj2DOther;

	OrientedRectangle* collider2DOther;

	bool hoveredPress;

	std::function<void(int)>* wPressed;

	std::function<void(int)>* wRelease;

	std::function<void(int)>* wPress;

	std::function<void(int)>* iPress;

	std::function<void(int)>* iRelease;
};

#endif // PLAYER_H