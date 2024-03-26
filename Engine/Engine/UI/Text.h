#ifndef TEXT_H
#define TEXT_H

#include "UserInterfaceItem.h"

#include <string>
#include <vector>
#include <unordered_set>

#include <glm/glm.hpp>

class UserInterfaceItem;
class Model;

class Text
{
public:

	static void LoadFonts();

	Text(const std::string& initialText, const glm::vec2& initialPosition = glm::vec2(100.0f, 600.0f), const float& initialSize = 25.0f, const std::string& initialFont = "Default", float horizontalSpacing = 10.0f, float verticalSpacing = 0.0f);

	~Text();

	Text(const Text&) = delete;

	Text& operator=(const Text&) = delete;

	Text(Text&&) = delete;

	Text& operator=(Text&&) = delete;

	float GetSize() const;

	void SetSize(float newSize);

	const std::string& Append(const std::string& postfix);

	const std::string& Prepend(const std::string& prefix);

	void SetPosition(const glm::vec2& newPosition);

	void SetVisibility(UserInterfaceItem::Visibility visibility);

protected:

	Text() = delete;

	void ContainCharacterModels(const std::string& characters, std::vector<Model*>& returnedCharacterModels, std::vector<std::string>& returnedCharacterModelNames) const;

private:

	void AddCharacterModelsAsUserInterfaceSubItems(const std::vector<std::string>& characterModelNames, const std::vector<Model*>& characterModels, bool appendOrPrepend = true);

	static std::vector<std::string> fontDirectories;

	static std::unordered_set<std::string> fontPrefixes;

	static std::vector<std::string> characterNames;

	glm::vec2 position;

	float horizontalSpacing;

	float verticalSpacing;

	float size;

	std::string fontName;

	std::string characters;

	std::vector<UserInterfaceItem*> characterUserInterfaceItems;

	UserInterfaceItem* textItem;

public:

};

#endif // TEXT_H