#include "Texture.h"

#include "../../Utils/Logger.h"
#include "../Memory/StagingBuffer.h"
#include "../Memory/Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb-master/stb_image.h>

Texture::Texture() :
	path("../Engine/Engine/Renderer/Images/texture.jpg")
{
	LoadTexture();
}

Texture::Texture(const std::string& p) :
	path(p)
{
	LoadTexture();
}

Texture::~Texture()
{
	delete image;
}

const Image& Texture::GetImage() const
{
	return *image;
}

void Texture::LoadTexture()
{
	stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);

	if (!pixels)
	{
		Logger::Log(std::string("Failed to load image ") + path, Logger::Category::Error);
		return;
	}

	StagingBuffer stagingBuffer(width * height * 4);
	stagingBuffer.Map(pixels, stagingBuffer.Size());

	image = new Image(width, height, stagingBuffer, 1);
}
