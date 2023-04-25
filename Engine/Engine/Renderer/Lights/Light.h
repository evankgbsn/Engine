#ifndef LIGHT_H
#define LIGHT_H

class Light
{
public:

	~Light();

protected:

	Light();

	Light(const Light&) = delete;

	Light& operator=(const Light&) = delete;

	Light(Light&&) = delete;

	Light& operator=(Light&&) = delete;

private:


};

#endif //LIGHT_H