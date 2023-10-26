#ifndef USERINTERFACEITEM_H
#define USERINTERFACEITEM_H

#include <unordered_map>
#include <string>

#include <glm/glm.hpp>

class Model;
class Texture;
class TexturedStatic2DGraphicsObject;

class UserInterfaceItem
{

public:

	UserInterfaceItem(Model* const model, Texture* const texture);

	~UserInterfaceItem();

	UserInterfaceItem(const UserInterfaceItem&) = delete;

	UserInterfaceItem& operator=(const UserInterfaceItem&) = delete;

	UserInterfaceItem(UserInterfaceItem&&) = delete;

	UserInterfaceItem& operator=(UserInterfaceItem&&) = delete;

	void AddSubItem(const std::string& name, UserInterfaceItem* const subItem);

	UserInterfaceItem* RemoveSubItem(const std::string& name);

	UserInterfaceItem* const GetSubItem(std::string& name) const;

protected:

private:

	UserInterfaceItem() = delete;

	glm::vec2 position;

	float angle;

	TexturedStatic2DGraphicsObject* graphicsObject;

	std::unordered_map<std::string, UserInterfaceItem*> subItems;

};

#endif // USERINTERFACEITEM_H
