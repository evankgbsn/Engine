#ifndef TEXT_H
#define TEXT_H

#include <string>
#include <vector>

class UserInterfaceItem;

class Text
{
public:

	static void LoadFonts();

	Text(const std::string& initialText);

	Text();

	~Text();

	Text(const Text&) = delete;

	Text& operator=(const Text&) = delete;

	Text(Text&&) = delete;

	Text& operator=(Text&&) = delete;

protected:

private:

	static std::vector<std::string> fontDirectories;

	static std::vector<std::string> fontPrefix;

	UserInterfaceItem* textItem;

};

#endif // TEXT_H