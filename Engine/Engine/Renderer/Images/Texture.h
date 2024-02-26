#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

class Image;

class Texture
{

public:

	Texture();

	Texture(const std::string& path, unsigned int imageBinding);

	~Texture();

	Texture(const Texture&) = delete;

	Texture& operator=(const Texture&) = delete;

	Texture(Texture&&) = delete;

	Texture& operator=(Texture&&) = delete;

	const Image& GetImage() const;

	void SetBinding(unsigned int binding);

	unsigned int GetBinding() const;

	int GetWidth() const;

	int GetHeight() const;

private:

	void LoadTexture();

	std::string path;

	int width;

	int height;

	int channels;

	Image* image;

	unsigned int binding;
};


#endif // TEXTURE_H