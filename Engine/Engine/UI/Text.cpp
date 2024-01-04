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
				Model* const loadedModel = ModelManager::LoadModel(directoryName + dirEntry.path().filename().string(), dirEntry.path().string());

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
	fontName(),
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
	for (unsigned int index = static_cast<unsigned int>(characterUserInterfaceItems.size()); index >= 0; index++)
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

const std::string& Text::Append(const std::string& postfix)
{
	std::vector<Model*> charactersAsModels(postfix.size());
	std::vector<std::string> charactersModelNames(postfix.size());

	std::string characterModelName("");

	for (const auto& character : postfix)
	{
		charactersModelNames.push_back(characterModelName = fontName + character);
		charactersAsModels.push_back(ModelManager::GetModel(characterModelName));
	}

	AddCharacterModelsAsUserInterfaceSubItems(charactersModelNames, charactersAsModels);
	
	return characters;
}

const std::string& Text::Prepend(const std::string& prefix)
{
	return characters;
}

Model* const Text::CharacterToModel() const
{
	return nullptr;
}

void Text::AddCharacterModelsAsUserInterfaceSubItems(const std::vector<std::string>& charactersModelName, const std::vector<Model*>& characterModels)
{
	for (unsigned int i = 0; i < characterModels.size(); i++)
	{
		UserInterfaceItem* characterUserInterfaceItem = new UserInterfaceItem(charactersModelName[i], characterModels[i], TextureManager::GetTexture("DefaultFontTexture"), {(horizontalSpacing * i) + position.x, (verticalSpacing * i) + position.y});

		characterUserInterfaceItems.push_back(characterUserInterfaceItem);

		textItem->AddSubItem(charactersModelName[i], characterUserInterfaceItem);
	}
}
