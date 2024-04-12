#ifndef PLAYER_H
#define PLAYER_H

#include "Entity/GameObject.h"

#include<functional>

class TexturedAnimatedGraphicsObject;
class ColoredStaticGraphicsObject;
class BoundingSphere;
class OrientedBoundingBox;
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

	Model* model;

	Texture* texture;

	TexturedAnimatedGraphicsObject* graphics;

	struct OBB
	{
		OrientedBoundingBox* obb;
		ColoredStaticGraphicsObject* graphics;
		unsigned int jointIndex;
	};

	std::vector<OBB*> obbs;

	OBB* otherObb;

	bool hoveredPress;

	std::function<void(int)>* wPressed;

	std::function<void(int)>* wRelease;

	std::function<void(int)>* wPress;

	std::function<void(int)>* iPress;

	std::function<void(int)>* iRelease;
};

#endif // PLAYER_H