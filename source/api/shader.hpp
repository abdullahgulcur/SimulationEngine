#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <vector>

#include "gl/glew.h"

struct File;

namespace ShaderNS {

	class ShaderFile {

	public:

		File* fileAddr;
		std::vector<std::string> sampler2DNames;
		std::vector<std::string> floatNames;

		ShaderFile();

		ShaderFile(File* file, std::string path);
	};

	unsigned int loadShaders(const char* vertex_file_path, const char* fragment_file_path);

	unsigned int loadShaders(const char* vertex_file_path, const char* fragment_file_path,
		const char* tess_control_file_path, const char* tess_evaluation_file_path);

	void parseShaderFile(const char* path, std::vector<std::string>& sampler2DNames, std::vector<std::string>& floatNames);

}