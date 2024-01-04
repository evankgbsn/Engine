#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include <unordered_map>
#include <unordered_set>
#include <string>

class Texture;

class TextureManager
{
public:

	static void Initialize();

	static void Terminate();

	static Texture* const LoadTexture(const std::string& filePath, const std::string& name);

	static Texture* const GetTexture(const std::string& name);

	static void UnloadTexture(const std::string& name);

private:

	TextureManager();

	~TextureManager();

	TextureManager(const TextureManager&) = delete;

	TextureManager& operator=(const TextureManager&) = delete;

	TextureManager(TextureManager&&) = delete;

	TextureManager& operator=(TextureManager&&) = delete;

	void LoadDefaultTextures();

	static TextureManager* instance;

	static std::unordered_set<std::string> defaultTextureNames;

	std::unordered_map<std::string, Texture*> textures;
};


#endif // TEXTUREMANAGER_H
