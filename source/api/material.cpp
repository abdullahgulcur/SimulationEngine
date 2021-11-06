#include "material.hpp"

Material::Material() {

	type = MaterialType::pbr;
	albedoColor = glm::vec3(1.0f, 1.0f, 1.0f);

	//programID = LoadShaders("source/shader/PBR.vertex", "source/shader/PBR.frag");

	//Material::loadUniforms();
}

void Material::compileShaders() {

	programID = LoadShaders("source/shader/PBR.vertex", "source/shader/PBR.frag");
	Material::loadUniforms();
}

void Material::loadUniforms() {

	mID = glGetUniformLocation(programID, "M");
	vID = glGetUniformLocation(programID, "V");
	pID = glGetUniformLocation(programID, "P");

	albedoColorID = glGetUniformLocation(programID, "albedoColor");
	albedoTextureID = glGetUniformLocation(programID, "albedoMap");
	normalTextureID = glGetUniformLocation(programID, "normalMap");
	metallicTextureID = glGetUniformLocation(programID, "metallicMap");
	roughnessTextureID = glGetUniformLocation(programID, "roughnessMap");
	aoTextureID = glGetUniformLocation(programID, "aoMap");

	normalAmountID = glGetUniformLocation(programID, "normal_amount");
	metallicAmountID = glGetUniformLocation(programID, "metallic_amount");
	roughnessAmountID = glGetUniformLocation(programID, "roughness_amount");
	aoAmountID = glGetUniformLocation(programID, "ao_amount");

	camPosID = glGetUniformLocation(programID, "camPos");
}

GLuint Material::LoadShaders(const char* vertex_file_path, const char* fragment_file_path) {

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
		return 0;
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
	std::string macro = "#version 330 core\n#define USE_ALBEDO " + std::to_string(useAlbedo) + "\n"; //1\n";
	macro += "#define USE_NORMAL " + std::to_string(useNormal) + "\n";
	macro += "#define USE_METALLIC " + std::to_string(useMetallic) + "\n";
	macro += "#define USE_ROUGHNESS " + std::to_string(useRoughness) + "\n";
	macro += "#define USE_AO " + std::to_string(useAO) + "\n";

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

	return ProgramID;
}

void Material::deleteProgram() {

	glDeleteProgram(programID);
}