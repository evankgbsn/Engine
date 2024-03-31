#ifndef COLLISIONMANAGER_H
#define COLLISIONMANAGER_H

class CollisionManager
{
public:

	static void Initialize();

	static void Terminate();

	static void Update();

private:

	CollisionManager();

	~CollisionManager();

};


#endif // COLLISIONMANAGER_H