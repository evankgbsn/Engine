#ifndef COLLIDER_H
#define COLLIDER_H

class Collider
{
public:
	
protected:

	Collider();

	~Collider();

	virtual void Update() = 0;

	virtual void ToggleVisibility() = 0;

private:

	Collider(const Collider&) = delete;

	Collider& operator=(const Collider&) = delete;
};

#endif // COLLIDER_H