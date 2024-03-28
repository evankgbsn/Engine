#ifndef PLAYER_H
#define PLAYER_H

#include "Entity/GameObject.h"

#include<functional>

class TexturedAnimatedGraphicsObject;

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

	TexturedAnimatedGraphicsObject* player;

	float moveSpeed;

	std::function<void(int)>*  wPress;
	std::function<void(int)>*  aPress;
	std::function<void(int)>*  sPress;
	std::function<void(int)>*  dPress;

	std::function<void(int)>* setClipOnMoveRelease;
	std::function<void(int)>* setClipOnMovePress;
	
	std::function<void(int)>* setClipOnJumpPress;
};

#endif // PLAYER_H