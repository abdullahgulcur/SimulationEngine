#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <vector>

#include "gl/glew.h"

namespace Shader {

	unsigned int loadShaders(const char* vertex_file_path, const char* fragment_file_path);

}