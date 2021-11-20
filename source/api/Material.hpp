#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

struct File;

namespace MaterialNS {

	class MaterialFile {

	public:

		// Real Material Address

		File* fileAddr;
		File* vertShaderFileAddr;
		File* fragShaderFileAddr;

		std::vector<File*> textureUnitFileAddrs;
		std::vector<unsigned int> textureUnits;

		std::vector<float> floatUnits;

		unsigned int programID;

		MaterialFile();

		MaterialFile(const char* vertex_file_path, const char* fragment_file_path);

		MaterialFile(File* file, File* vertShaderFileAddr, File* fragShaderFileAddr, const char* vertex_file_path, const char* fragment_file_path, int dirLightCount, int pointLightCount);

		void compileShaders(const char* vertex_file_path, const char* fragment_file_path, int dirLightCount, int pointLightCount);

		void deleteProgram();
	};
}

