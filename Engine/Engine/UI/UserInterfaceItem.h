#ifndef USERINTERFACEITEM_H
#define USERINTERFACEITEM_H

#include <unordered_map>
#include <string>
#include <functional>

#include <glm/glm.hpp>

class Model;
class Texture;
class TexturedStatic2DGraphicsObject;

class UserInterfaceItem
{

public:

	UserInterfaceItem(const std::string& name, Model* const model, Texture* const texture, const glm::vec2& initialPosition);

	~UserInterfaceItem();

	UserInterfaceItem(const UserInterfaceItem&) = delete;

	UserInterfaceItem& operator=(const UserInterfaceItem&) = delete;

	UserInterfaceItem(UserInterfaceItem&&) = delete;

	UserInterfaceItem& operator=(UserInterfaceItem&&) = delete;

	void AddSubItem(const std::string& name, UserInterfaceItem* const subItem);

	UserInterfaceItem* RemoveSubItem(const std::string& name);

	UserInterfaceItem* const GetSubItem(std::string& name) const;

	const std::unordered_map<std::string, UserInterfaceItem*>& GetSubItems() const;

	void Hovered(std::function<void()> onHover) const;

	void Scale(float x, float y);

	void Rotate(float angle);

	void Translate(float x, float y);
	
	void OnWindowSizeUpdate();

	const glm::vec2& GetTranslation() const;

	bool TransformReady(std::function<void()>& whenTransformReady = emptyFunctionObject);

protected:

private:

	static std::function<void()> emptyFunctionObject;

	bool transformReady;

	UserInterfaceItem() = delete;

	std::unordered_map<std::string, UserInterfaceItem*> subItems;

	std::string name;

	std::function<void()> whenTransformReady;

	TexturedStatic2DGraphicsObject* graphicsObject;

	glm::vec2 position;

	float angle;
	
};

#endif // USERINTERFACEITEM_H
