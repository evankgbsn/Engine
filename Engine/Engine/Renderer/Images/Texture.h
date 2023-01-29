#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

class Image;

class Texture
{

public:

	Texture();

	Texture(const std::string& path);

	~Texture();

	Texture(const Texture&) = delete;

	Texture& operator=(const Texture&) = delete;

	Texture(Texture&&) = delete;

	Texture& operator=(Texture&&) = delete;

	const Image& GetImage() const;

private:

	void LoadTexture();

	std::string path;

	int width;

	int height;

	int channels;

	Image* image;

};


#endif // TEXTURE_H