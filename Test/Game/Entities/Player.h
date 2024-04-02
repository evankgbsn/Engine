#ifndef PLAYER_H
#define PLAYER_H

#include "Entity/GameObject.h"

#include<functional>

class TexturedAnimatedGraphicsObject;
class BoundingSphere;
class OrientedBoundingBox;
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

	Model* model;

	Texture* texture;

	TexturedAnimatedGraphicsObject* graphics;

	std::vector<OrientedBoundingBox*> boundingSpheres;

	OrientedBoundingBox* collider;

	std::function<void(int)>* wPressed;

	std::function<void(int)>* wRelease;

	std::function<void(int)>* wPress;
};

#endif // PLAYER_H