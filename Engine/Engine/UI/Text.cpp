#include "Text.h"

#include "UserInterfaceManager.h"
#include "UserInterfaceItem.h"
#include "../Renderer/Model/ModelManager.h"
#include "../Renderer/Model/Model.h"
#include "../Renderer/Images/TextureManager.h"
#include "../Utils/Logger.h"

#include <filesystem>
#include <unordered_map>

std::vector<std::string> Text::fontDirectories = std::vector<std::string>({ "../Engine/Engine/Renderer/Model/Fonts/Default/" });
std::unordered_set<std::string> Text::fontPrefixes = std::unordered_set<std::string>();

void Text::LoadFonts()
{
	for (const std::string& fontDirectory : fontDirectories)
	{
		std::string directoryName = "";

		for (unsigned int i = static_cast<unsigned int>(fontDirectory.size() - size_t(2)); i > 0; i--)
		{
			if (fontDirectory[i] == '/')
			{
				break;
			}

			directoryName = fontDirectory[i] + directoryName;
		}

		fontPrefixes.insert(directoryName);

		for (auto& dirEntry : std::filesystem::directory_iterator(fontDirectory))
		{
			if (dirEntry.path().extension().string() == ".gltf")
			{
				std::string modelFilePath = directoryName + dirEntry.path().filename().string();
				std::string newCharacterModelName(modelFilePath.replace(modelFilePath.begin() + modelFilePath.size() - 5, modelFilePath.end(), ""));

				Model* loadedModel = ModelManager::LoadModel(newCharacterModelName, dirEntry.path().string());

				loadedModel->FlipTriangleWindingOrder();

				if (loadedModel != nullptr)
				{
					loadedModel->SetZforAllVerts(0.2f);
				}
			}
		}
	}
}

Text::Text(const std::string& initialText, const glm::vec2& initialPosition, float zOrder, const float& initialSize, const std::string& initialFont, float spacingH, float spacingV) :
	position(initialPosition),
	cursor(glm::vec2(position.x, position.y)),
	horizontalSpacing(spacingH),
	verticalSpacing(spacingV),
	size(initialSize),
	fontName("Default"),
	characters(),
	characterUserInterfaceItems(),
	textItem(nullptr),
	modelNameCounts(std::unordered_map<std::string, unsigned int>())
{
	if (fontPrefixes.find(initialFont) == fontPrefixes.end())
		fontPrefixes.size() > 0 ? fontName = *fontPrefixes.begin() : fontName = "";

	if (fontName.empty())
		Logger::Log(std::string("No fonts available for Text instance."), Logger::Category::Error);

	Append(initialText, zOrder);
}

Text::~Text()
{
	for (unsigned int index = static_cast<unsigned int>(characterUserInterfaceItems.size()); index != 0; index--)
	{
		delete characterUserInterfaceItems[index];
	}

	characterUserInterfaceItems.clear();
}

float Text::GetSize() const
{
	return size;
}

void Text::SetSize(float newSize)
{
	if (!characters.empty())
	{
		const std::unordered_map<std::string, UserInterfaceItem*>& subCharacterItems = textItem->GetSubItems();

		for (const std::pair<std::string, UserInterfaceItem*>& characterSubItem : subCharacterItems)
		{
			glm::vec2 currentScale = characterSubItem.second->GetScale();

			float scaleX = 1.0f / currentScale.x;
			float scaleY = 1.0f / currentScale.y;

			characterSubItem.second->Scale(scaleX, scaleY);
			characterSubItem.second->Scale(newSize, newSize);
		}

		size = newSize;
	}
}

void Text::ContainCharacterModels(const std::string& characters, std::vector<Model*>& returnedCharacterModels, std::vector<std::string>& returnedCharacterModelNames) const
{
	std::string characterModelName("");
	Model* unassignedModel = nullptr;

	std::unordered_set<char> containerOpenCharacterUSACIIDecimalValues = {(char)40, (char)60, (char)91, (char)123};
	std::unordered_set<char> containerCloseCharacterUSACIIDecimalValues = {(char)41, (char)62, (char)93, (char)125};
	std::unordered_set<char> containerBeginAndEndCharacterUSACIIDecimalValues = {(char)65, (char)90, (char)97, (char)122};

	for (unsigned int i = 0; i < characters.size(); i++)
	{
		const char character = (char)characters.at(i);

		std::string prefix = "";
		if (character >= *containerBeginAndEndCharacterUSACIIDecimalValues.find('a') && character <= *containerBeginAndEndCharacterUSACIIDecimalValues.find('z'))
		{
			prefix = std::string("Lowercase");
			const_cast<char&>(character) = std::toupper(character);
		}
		else if (character >= *containerBeginAndEndCharacterUSACIIDecimalValues.find('A') && character <= *containerBeginAndEndCharacterUSACIIDecimalValues.find('Z'))
		{
			prefix = std::string("Uppercase");
			const_cast<char&>(character) = std::toupper(character);
		}
		else
		{
			if (containerOpenCharacterUSACIIDecimalValues.contains(character))
			{
				prefix = "Open";
			}
			else if (containerCloseCharacterUSACIIDecimalValues.contains(character))
			{
				prefix = "Close";
			}
		}

		static std::unordered_map<char, std::string> signNames = std::unordered_map<char, std::string>({
			std::make_pair('[', std::string("Braket")),
			std::make_pair(']', std::string("Braket")),
			std::make_pair('(', std::string("Parenthese")),
			std::make_pair(')', std::string("Parenthese")),
			std::make_pair('<', std::string("MagnitudeEvaluation")),
			std::make_pair('>', std::string("MagnitudeEvaluation")),
			std::make_pair('?', std::string("Question")),
			std::make_pair(';', std::string("SemiColon")),
			std::make_pair(':', std::string("Colon")),
			std::make_pair('/"', std::string("Quotation")),
			std::make_pair('@', std::string("At")),
			std::make_pair('&', std::string("Ampersand")),
			std::make_pair('*', std::string("Asterisk")),
			std::make_pair('\\', std::string("Backward")),
			std::make_pair('/', std::string("Forward")),
			std::make_pair('|', std::string("Pipe")),
			std::make_pair('$', std::string("Dollar")),
			std::make_pair(',', std::string("Comma")),
			std::make_pair('^', std::string("Carrot")),
			std::make_pair('=', std::string("Equal")),
			std::make_pair('!', std::string("Exclaimation")),
			std::make_pair('.', std::string("Period")),
			std::make_pair('%', std::string("Percent")),
			std::make_pair('0', std::string("Zero")),
			std::make_pair('1', std::string("One")),
			std::make_pair('2', std::string("Two")),
			std::make_pair('3', std::string("Three")),
			std::make_pair('4', std::string("Four")),
			std::make_pair('5', std::string("Five")),
			std::make_pair('6', std::string("Six")),
			std::make_pair('7', std::string("Seven")),
			std::make_pair('8', std::string("Eight")),
			std::make_pair('9', std::string("Nine")),
			std::make_pair('#', std::string("Pound")),
			std::make_pair('-', std::string("Minus")),
			std::make_pair('+', std::string("Plus"))
		});

		returnedCharacterModelNames[i] = characterModelName = signNames.contains(character) ? fontName + prefix += (*signNames.find(character)).second : fontName + prefix + character;
		returnedCharacterModels[i] = ((unassignedModel = ModelManager::GetModel(characterModelName)) != nullptr) ? unassignedModel : ModelManager::GetModel("DefaultRectangleWithDepth");
	}
}

const std::string& Text::Append(const std::string& postfix, float zOrder)
{
	if (!postfix.empty())
	{
		if (postfix == " ")
		{
			cursor.x += horizontalSpacing;
		}
		else
		{
			std::vector<Model*> charactersAsModels(postfix.size());
			std::vector<std::string> charactersModelNames(postfix.size());

			ContainCharacterModels(postfix, charactersAsModels, charactersModelNames);

			AddCharacterModelsAsUserInterfaceSubItems(charactersModelNames, charactersAsModels, zOrder, true);
		}
	}
	
	return characters = characters + postfix;
}

const std::string& Text::Prepend(const std::string& prefix)
{
	std::vector<Model*> charactersAsModels(prefix.size());
	std::vector<std::string> charactersModelNames(prefix.size());

	ContainCharacterModels(prefix, charactersAsModels, charactersModelNames);

	AddCharacterModelsAsUserInterfaceSubItems(charactersModelNames, charactersAsModels, false);

	return characters = prefix + characters;
}

void Text::SetPosition(const glm::vec2& newPosition)
{
	unsigned int i = 0;
	for (UserInterfaceItem* item : characterUserInterfaceItems)
	{
		item->SetPosition(newPosition.x + horizontalSpacing * i++, newPosition.y);
	}

	cursor.x = newPosition.x + horizontalSpacing * characterUserInterfaceItems.size();
	cursor.y = newPosition.y;
}

void Text::SetVisibility(UserInterfaceItem::Visibility newVisibility)
{
	if (textItem != nullptr)
	{
		const std::unordered_map<std::string, UserInterfaceItem*>& subCharacterItems = textItem->GetSubItems();

		if (!subCharacterItems.empty())
		{
			for (const std::pair<std::string, UserInterfaceItem*>& characterSubItem : subCharacterItems)
			{
				characterSubItem.second->InquireVisibility(newVisibility);
			}
		}

		textItem->InquireVisibility(newVisibility);
	}
}

void Text::SetZOrder(float newZ)
{
	for (UserInterfaceItem* item : characterUserInterfaceItems)
	{
		item->SetZOrder(newZ);
	}
}

float Text::GetZOrder() const
{
	if (!characterUserInterfaceItems.empty())
	{
		return characterUserInterfaceItems[0]->GetZOrder();
	}

	return 10.0f;
}

void Text::Backspace()
{
	if (characters.size() > 0)
	{
		if (!characterUserInterfaceItems.empty() && characters.at(characters.size() - 1) != ' ')
		{
			delete characterUserInterfaceItems.at(characterUserInterfaceItems.size() - 1);
			characterUserInterfaceItems.erase(std::next(characterUserInterfaceItems.begin(), characterUserInterfaceItems.size() - 1));
		}
		
		cursor.x += -horizontalSpacing;

		if (characters.size() == 1)
		{
			textItem = nullptr;
		}

		std::string charactersBefore = characters;

		characters.erase(std::next(characters.begin(), characters.size() - 1));
		Logger::Log(characters + " : " + charactersBefore, Logger::Category::Warning);
	}
}

void Text::AddCharacterModelsAsUserInterfaceSubItems(const std::vector<std::string>& charactersModelName, const std::vector<Model*>& characterModels, float z, bool appendOrPrepend)
{
	auto createSubItemName = [this, &charactersModelName](unsigned int index) -> std::string
		{
			modelNameCounts[charactersModelName.at(index)]++;
			char itoaChar[] = { '\0', '\0' , '\0' , '\0' , '\0' };
			_itoa_s(modelNameCounts[charactersModelName.at(index)], itoaChar, 10);
			return charactersModelName.at(index) + itoaChar;
		};

	std::string firstCharacterName = createSubItemName(0);

	UserInterfaceItem* firstAppendedCharacterModelAsUserInterfaceSubItem;
	UserInterfaceItem* lastCreatedTextItem = firstAppendedCharacterModelAsUserInterfaceSubItem = new UserInterfaceItem(charactersModelName[0], characterModels[0], TextureManager::GetTexture("DefaultFontTexture"), z, cursor, {size, size});

	if (textItem == nullptr)
		textItem = lastCreatedTextItem;	

	characterUserInterfaceItems.push_back(lastCreatedTextItem);

	for (unsigned int i = 1; i < characterModels.size(); i++)
	{
		cursor.x += horizontalSpacing;
		cursor.y += verticalSpacing;

		std::string subItemName = createSubItemName(i);

		UserInterfaceItem* characterUserInterfaceItem = new UserInterfaceItem(subItemName, characterModels.at(i), TextureManager::GetTexture("DefaultFontTexture"), z, cursor, {size, size});

		characterUserInterfaceItems.push_back(characterUserInterfaceItem);

		textItem->AddSubItem(subItemName, characterUserInterfaceItem);

		lastCreatedTextItem = characterUserInterfaceItem;
	}

	cursor.x += horizontalSpacing;
	cursor.y += verticalSpacing;

	if (appendOrPrepend && textItem != lastCreatedTextItem)
	{
		textItem->AddSubItem(firstCharacterName, firstAppendedCharacterModelAsUserInterfaceSubItem);
		return;
	}

	const std::unordered_map<std::string, UserInterfaceItem*>& subCharacterItems = textItem->GetSubItems();

	glm::vec2 translationIncrementor = textItem->GetTranslation();
	for (const std::pair<std::string, UserInterfaceItem*>& characterSubItem : subCharacterItems)
	{
		characterSubItem.second->Translate(translationIncrementor.x += horizontalSpacing, translationIncrementor.y += verticalSpacing);
	}
}
