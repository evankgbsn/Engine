#include "Texture.h"

#include "../../Utils/Logger.h"
#include "../Memory/StagingBuffer.h"
#include "../Memory/Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb-master/stb_image.h>

Texture::Texture() :
	path("../Engine/Engine/Renderer/Images/Woman.png"),
	binding(1)
{
	LoadTexture();
}

Texture::Texture(const std::string& p, unsigned int imageBinding) :
	path(p),
	binding(imageBinding)
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

void Texture::SetBinding(unsigned int binding)
{
	image->SetBinding(binding);
}

unsigned int Texture::GetBinding() const
{
	return image->Binding();
}

int Texture::GetWidth() const
{
	return width;
}

int Texture::GetHeight() const
{
	return height;
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

	free(pixels);

	image = new Image(width, height, stagingBuffer, binding);
}
