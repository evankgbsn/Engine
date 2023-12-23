#include "Text.h"

#include "UserInterfaceManager.h"
#include "UserInterfaceItem.h"
#include "../Renderer/Model/ModelManager.h"
#include "../Renderer/Model/Model.h"

#include <filesystem>

std::vector<std::string> Text::fontDirectories = std::vector<std::string>({ "../Engine/Engine/Renderer/Model/Fonts/Default/" });

void Text::LoadFonts()
{
	for (const std::string& fontDirectory : fontDirectories)
	{
		std::string directoryName = "";

		for (unsigned int i = fontDirectory.size() - 2; i > 0; i--)
		{
			if (fontDirectory[i] == '/')
			{
				break;
			}

			directoryName = fontDirectory[i] + directoryName;
		}


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

Text::Text(const std::string& initialText) :
	textItem(nullptr)
{
}

Text::Text() :
	textItem(nullptr)
{
}

Text::~Text()
{
}
