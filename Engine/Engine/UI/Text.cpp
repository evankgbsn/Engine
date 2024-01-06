#include "Text.h"

#include "UserInterfaceManager.h"
#include "UserInterfaceItem.h"
#include "../Renderer/Model/ModelManager.h"
#include "../Renderer/Model/Model.h"
#include "../Renderer/Images/TextureManager.h"
#include "../Utils/Logger.h"

#include <filesystem>

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

				Model* const loadedModel = ModelManager::LoadModel(newCharacterModelName, dirEntry.path().string());

				if (loadedModel != nullptr)
				{
					loadedModel->SetZforAllVerts(0.2f);
				}
			}
		}
	}
}

Text::Text(const std::string& initialText, const glm::vec2& initialPosition, const std::string& initialFont, float spacingH, float spacingV) :
	position(initialPosition),
	horizontalSpacing(spacingH),
	verticalSpacing(spacingV),
	size(),
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
	size = newSize;
}

void Text::ContainCharacterModels(const std::string& characters, std::vector<Model*>& returnedCharacterModels, std::vector<std::string>& returnedCharacterModelNames) const
{
	std::string characterModelName("");
	Model* unassignedModel = nullptr;

	std::unordered_set<unsigned int> containerOpenCharacterUSACIIDecimalValues = {40, 60, 91, 123};
	std::unordered_set<unsigned int> containerCloseCharacterUSACIIDecimalValues = {41, 62, 93, 125};

	for (unsigned int i = 0; i < characters.size(); i++)
	{
		std::string prefix = "";
		if ((char)characters.at(i) >= (char)65 && characters.at(i) <= (char)90)
		{
			prefix = std::string("Uppercase");
			const_cast<char&>(characters.at(i)) = (char)std::toupper((char)characters.at(i));
		}
		else if ((char)characters.at(i) >= (char)97 && characters.at(i) <= (char)122)
		{
			prefix = std::string("Lowercase");
			const_cast<char&>(characters.at(i)) = (char)std::toupper((char)characters.at(i));
		}
		else
		{
			if (containerOpenCharacterUSACIIDecimalValues.contains(characters.at(i)))
			{
				prefix = "Open";
			}
			else if (containerOpenCharacterUSACIIDecimalValues.contains(characters.at(i)))
			{
				prefix = "Close";
			}
		}

		returnedCharacterModelNames[i] = characterModelName = fontName + prefix + characters.at(i);
		returnedCharacterModels[i] = ((unassignedModel = ModelManager::GetModel(characterModelName)) != nullptr) ? unassignedModel : ModelManager::GetModel("DefaultRectangleWithDepth");
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
