#pragma once

#include <stdio.h>
#include <vector>
#include <string>
#include <stdlib.h>

#include <GL/glew.h>

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

enum class TextureType{ albedo, normal, metallic, roughness, ao};

class Texture {

private:

public:

	unsigned int textureID;
	std::string name;

	Texture();

	unsigned int loadDDS(const char* imagepath);

	void setTextureID(const char* imagepath);

	void setEmptyTextureID();

	GLuint getEmptyTexture(); // make it static

};