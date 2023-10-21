#ifndef Skybox_H
#define Skybox_H

#include "Entity/GameObject.h"

class Skybox : public GameObject
{
public:

	Skybox();

	~Skybox();

	void Update() final;

private:

	Skybox(const Skybox&) = delete;

	Skybox& operator=(const Skybox&) = delete;

	Skybox(Skybox&&) = delete;

	Skybox& operator=(Skybox&&) = delete;

};


#endif // Skybox_H