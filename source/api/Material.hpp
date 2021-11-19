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

		File* fileAddr;
		int vertShaderFileID;
		int fragShaderFileID;

		std::vector<std::string> textureUnitPaths; // file ids
		std::vector<unsigned int> textureUnits;

		std::vector<float> floatUnits;

		unsigned int programID;

		MaterialFile();

		MaterialFile(const char* vertex_file_path, const char* fragment_file_path);

		MaterialFile(File* file, int vertShaderFileID, int fragShaderFileID, const char* vertex_file_path, const char* fragment_file_path, int dirLightCount, int pointLightCount);

		void compileShaders(const char* vertex_file_path, const char* fragment_file_path, int dirLightCount, int pointLightCount);

		void deleteProgram();
	};
}

