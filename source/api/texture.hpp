#pragma once

#include <GL/glew.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

enum class TextureType{ albedo, normal, metallic, roughness, ao};

class Texture {

private:

public:

	Texture();

	unsigned int loadDDS(const char* imagepath);

	GLuint getEmptyTexture();

};