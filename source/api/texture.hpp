#pragma once

#include <stdio.h>
#include <vector>
#include <string>
#include <stdlib.h>

#include <GL/glew.h>

#include <lodepng/lodepng.h>

struct File;

namespace TextureNS {

#define FOURCC_DXT1 0x31545844
#define FOURCC_DXT3 0x33545844
#define FOURCC_DXT5 0x35545844

	enum class TextureType { albedo, normal, metallic, roughness, ao };

	unsigned int getEmptyTexture();

	unsigned int loadDDS(const char* imagepath);

	unsigned char* loadBMP(const char* imagepath);

	unsigned char* loadPNG(const char* imagepath, unsigned& width, unsigned& height);

	class TextureFile {

	private:

	public:

		unsigned int textureID;
		File* fileAddr;

		TextureFile();

		TextureFile(const char* imagepath);

		TextureFile(File* file, const char* imagepath);

	};
}