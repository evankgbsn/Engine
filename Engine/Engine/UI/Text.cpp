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

Text::Text(const std::string& initialText, const glm::vec2& initialPosition, const float& initialSize, const std::string& initialFont, float spacingH, float spacingV) :
	position(initialPosition),
	horizontalSpacing(spacingH),
	verticalSpacing(spacingV),
	size(initialSize),
	fontName("Default"),
	characters(),
	characterUserInterfaceItems(),
	textItem(nullptr)
{
	if (fontPrefixes.find(initialFont) == fontPrefixes.end())
		fontPrefixes.size() > 0 ? fontName = *fontPrefixes.begin() : fontName = "";

	if (fontName.empty())
		Logger::Log(std::string("No fonts available for Text instance."), Logger::Category::Error);

	Append(initialText);

	SetSize(size);
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
	const std::unordered_map<std::string, UserInterfaceItem*>& subCharacterItems = textItem->GetSubItems();

	for (const std::pair<std::string, UserInterfaceItem*>& characterSubItem : subCharacterItems)
	{
		characterSubItem.second->Scale(newSize, newSize);
	}

	size = newSize;
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

		static std::unordered_map<char, std::string> signNames = std::unordered_map<char, std::string>();
		signNames.insert(std::make_pair('[', std::string("Braket")));
		signNames.insert(std::make_pair(']', std::string("Braket")));
		signNames.insert(std::make_pair('(', std::string("Parenthese")));
		signNames.insert(std::make_pair(')', std::string("Parenthese")));
		signNames.insert(std::make_pair('<', std::string("MagnitudeEvaluation")));
		signNames.insert(std::make_pair('>', std::string("MagnitudeEvaluation")));
		signNames.insert(std::make_pair('?', std::string("Question")));
		signNames.insert(std::make_pair(';', std::string("SemiColon")));
		signNames.insert(std::make_pair(':', std::string("Colon")));
		signNames.insert(std::make_pair('/"', std::string("Quotation")));
		signNames.insert(std::make_pair('@', std::string("At")));
		signNames.insert(std::make_pair('&', std::string("Ampersand")));
		signNames.insert(std::make_pair('*', std::string("Asterisk")));
		signNames.insert(std::make_pair('\\', std::string("Backward")));
		signNames.insert(std::make_pair('/', std::string("Forward")));
		signNames.insert(std::make_pair('|', std::string("Pipe")));
		signNames.insert(std::make_pair('$', std::string("Dollar")));
		signNames.insert(std::make_pair(',', std::string("Comma")));
		signNames.insert(std::make_pair('^', std::string("Carrot")));
		signNames.insert(std::make_pair('=', std::string("Equal")));
		signNames.insert(std::make_pair('!', std::string("Exclaimation")));
		signNames.insert(std::make_pair('.', std::string("Period")));
		signNames.insert(std::make_pair('%', std::string("Percent")));
		signNames.insert(std::make_pair('0', std::string("Zero")));
		signNames.insert(std::make_pair('1', std::string("One")));
		signNames.insert(std::make_pair('2', std::string("Two")));
		signNames.insert(std::make_pair('3', std::string("Three")));
		signNames.insert(std::make_pair('4', std::string("Four")));
		signNames.insert(std::make_pair('5', std::string("Five")));
		signNames.insert(std::make_pair('6', std::string("Six")));
		signNames.insert(std::make_pair('7', std::string("Seven")));
		signNames.insert(std::make_pair('8', std::string("Eight")));
		signNames.insert(std::make_pair('9', std::string("Nine")));
		signNames.insert(std::make_pair('#', std::string("Pound")));
		signNames.insert(std::make_pair('-', std::string("Minus")));
		signNames.insert(std::make_pair('+', std::string("Plus")));

		returnedCharacterModelNames[i] = characterModelName = signNames.contains(character) ? fontName + prefix += (*signNames.find(character)).second : fontName + prefix + character;
		returnedCharacterModels[i] = ((unassignedModel = ModelManager::GetModel(characterModelName)) != nullptr) ? unassignedModel : ModelManager::GetModel("DefaultRectangleWithDepth");
		//unassignedModel->FlipTriangleWindingOrder();

	}
}

const std::string& Text::Append(const std::string& postfix)
{
	std::vector<Model*> charactersAsModels(postfix.size());
	std::vector<std::string> charactersModelNames(postfix.size());

	ContainCharacterModels(postfix, charactersAsModels, charactersModelNames);

	AddCharacterModelsAsUserInterfaceSubItems(charactersModelNames, charactersAsModels, true);
	
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
}

void Text::SetVisibility(UserInterfaceItem::Visibility newVisibility)
{
	const std::unordered_map<std::string, UserInterfaceItem*>& subCharacterItems = textItem->GetSubItems();

	for (const std::pair<std::string, UserInterfaceItem*>& characterSubItem : subCharacterItems)
	{
		characterSubItem.second->InquireVisibility(newVisibility);
	}
}

void Text::AddCharacterModelsAsUserInterfaceSubItems(const std::vector<std::string>& charactersModelName, const std::vector<Model*>& characterModels, bool appendOrPrepend)
{
	UserInterfaceItem* firstAppendedCharacterModelAsUserInterfaceSubItem;
	UserInterfaceItem* lastCreatedTextItem = firstAppendedCharacterModelAsUserInterfaceSubItem = new UserInterfaceItem(charactersModelName[0], characterModels[0], TextureManager::GetTexture("DefaultFontTexture"), {position.x, position.y});

	if (textItem == nullptr)
		textItem = lastCreatedTextItem;

	for (unsigned int i = 1; i < characterModels.size(); i++)
	{
		UserInterfaceItem* characterUserInterfaceItem = new UserInterfaceItem(charactersModelName.at(i), characterModels.at(i), TextureManager::GetTexture("DefaultFontTexture"), {(horizontalSpacing * i) + position.x, (verticalSpacing * i) + position.y});

		characterUserInterfaceItems.push_back(characterUserInterfaceItem);

		textItem->AddSubItem(charactersModelName[i], characterUserInterfaceItem);

		lastCreatedTextItem = characterUserInterfaceItem;
	}

	if (appendOrPrepend)
	{
		textItem->AddSubItem(charactersModelName[0], firstAppendedCharacterModelAsUserInterfaceSubItem);
		return;
	}

	const std::unordered_map<std::string, UserInterfaceItem*>& subCharacterItems = textItem->GetSubItems();

	glm::vec2 translationIncrementor = textItem->GetTranslation();
	for (const std::pair<std::string, UserInterfaceItem*>& characterSubItem : subCharacterItems)
	{
		characterSubItem.second->Translate(translationIncrementor.x += horizontalSpacing, translationIncrementor.y += verticalSpacing);
	}
}
