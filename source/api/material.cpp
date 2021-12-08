#include "material.hpp"
#include "filesystem.hpp"
#include "meshrenderer.hpp"

Material::MaterialFile::MaterialFile() {}

Material::MaterialFile::MaterialFile(const char* vertex_file_path, const char* fragment_file_path){

	fileAddr = NULL;
	this->vertShaderFileAddr = NULL;
	this->fragShaderFileAddr = NULL;
	MaterialFile::compileShaders(vertex_file_path, fragment_file_path, 0, 0);
}

Material::MaterialFile::MaterialFile(File* file, File* vertShaderFileAddr, File* fragShaderFileAddr, const char* vertex_file_path, const char* fragment_file_path,
	int dirLightCount, int pointLightCount) {

	fileAddr = file;
	this->vertShaderFileAddr = vertShaderFileAddr;
	this->fragShaderFileAddr = fragShaderFileAddr;
	MaterialFile::compileShaders(vertex_file_path, fragment_file_path, dirLightCount, pointLightCount);
}

void Material::MaterialFile::compileShaders(const char* vertex_file_path, const char* fragment_file_path, int dirLightCount, int pointLightCount) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		printf("Could not open %s.\n", vertex_file_path);
		getchar();
		return;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	char const* fragmentShaderArr[2];
	std::string macro = "#version 330 core\n";
	macro += "#define DIR_LIGHT_COUNT " + std::to_string(dirLightCount) + "\n";
	macro += "#define POINT_LIGHT_COUNT " + std::to_string(pointLightCount) + "\n";

	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	fragmentShaderArr[0] = macro.c_str();
	fragmentShaderArr[1] = FragmentSourcePointer;

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	glShaderSource(FragmentShaderID, 2, fragmentShaderArr, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	programID = ProgramID;
}

void Material::MaterialFile::deleteProgram() {

	glDeleteProgram(programID);
}