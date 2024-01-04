#ifndef TEXT_H
#define TEXT_H

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

	Text(const std::string& initialText, const glm::vec2& initialPosition = glm::vec2(100.0f, 600.0f), const std::string& initialFont = "Default", float horizontalSpacing = 5.0f, float verticalSpacing = 5.0f);

	~Text();

	Text(const Text&) = delete;

	Text& operator=(const Text&) = delete;

	Text(Text&&) = delete;

	Text& operator=(Text&&) = delete;

	float GetSize() const;

	void SetSize(float newSize);

	const std::string& Append(const std::string& postfix);

	const std::string& Prepend(const std::string& prefix);

protected:

	Text() = delete;

private:

	Model* const CharacterToModel() const;

	void AddCharacterModelsAsUserInterfaceSubItems(const std::vector<std::string>& characterModelNames, const std::vector<Model*>& characterModels);

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