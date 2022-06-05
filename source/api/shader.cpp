#include "shader.hpp"
#include "filesystem.hpp"

ShaderNS::ShaderFile::ShaderFile() {

	fileAddr = NULL;
}

ShaderNS::ShaderFile::ShaderFile(File* file, std::string path) {

	fileAddr = file;
	ShaderNS::parseShaderFile(path.c_str(), sampler2DNames, floatNames);
}

void ShaderNS::parseShaderFile(const char* path, std::vector<std::string>& sampler2DNames, std::vector<std::string>& floatNames) {

	std::ifstream file(path);
	std::vector<std::string> names;

	if (file.is_open()) {

		std::string line;
		while (std::getline(file, line)) {
		
			char* token = strtok((char*)line.c_str(), " ");

			names.clear();

			while (token != NULL) {
				names.push_back(token);
				token = strtok(NULL, " ");
			}

			if (names.size() != 5)
				continue;

			if (names[0] == "uniform") {

				if (names[1] == "sampler2D")
					sampler2DNames.push_back(names[4]);
				else if (names[1] == "float" && names[2] != "dirLightPowers[DIR_LIGHT_COUNT];" &&
					names[2] != "pointLightPowers[POINT_LIGHT_COUNT];") {
					floatNames.push_back(names[4]);
				}
			}
			else
				continue;
		}

		file.close();
	}
}

unsigned int ShaderNS::loadShaders(const char* vertex_file_path, const char* fragment_file_path) {

	// Create the shaders
	unsigned int VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	unsigned int FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

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
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
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

	int Result = GL_FALSE;
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



	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
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
	unsigned int ProgramID = glCreateProgram();
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

unsigned int ShaderNS::loadShaders(const char* vertex_file_path, const char* fragment_file_path,
	const char* tess_control_file_path, const char* tess_evaluation_file_path) {

	// Create the shaders
	unsigned int VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	unsigned int FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	//unsigned int GeomShaderID = glCreateShader(GL_GEOMETRY_SHADER);
	unsigned int TessControlShaderID = glCreateShader(GL_TESS_CONTROL_SHADER);
	unsigned int TessEvaluationShaderID = glCreateShader(GL_TESS_EVALUATION_SHADER);

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
		printf("Impossible to open %s.\n", vertex_file_path);
		getchar();
		return 0;
	}



	//if (geom_file_path) {

	//	// Read the Geometry Shader code from the file
	//	std::string GeomShaderCode;
	//	std::ifstream GeomShaderStream(geom_file_path, std::ios::in);
	//	if (GeomShaderStream.is_open()) {
	//		std::stringstream sstr;
	//		sstr << GeomShaderStream.rdbuf();
	//		GeomShaderCode = sstr.str();
	//		GeomShaderStream.close();
	//	}
	//	else {
	//		printf("Impossible to open %s.\n", geom_file_path);
	//		getchar();
	//		return 0;
	//	}
	//}
	
	// Read the Tesselation Control Shader code from the file
	std::string TessControlShaderCode;
	std::ifstream TessControlShaderStream(tess_control_file_path, std::ios::in);
	if (TessControlShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << TessControlShaderStream.rdbuf();
		TessControlShaderCode = sstr.str();
		TessControlShaderStream.close();
	}
	else {
		printf("Impossible to open %s.\n", tess_control_file_path);
		getchar();
		return 0;
	}

	// Read the Tesselation Evaluation Shader code from the file
	std::string TessEvaShaderCode;
	std::ifstream TessEvaShaderStream(tess_evaluation_file_path, std::ios::in);
	if (TessEvaShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << TessEvaShaderStream.rdbuf();
		TessEvaShaderCode = sstr.str();
		TessEvaShaderStream.close();
	}
	else {
		printf("Impossible to open %s.\n", tess_evaluation_file_path);
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
	else {
		printf("Impossible to open %s.\n", fragment_file_path);
		getchar();
		return 0;
	}

	int Result = GL_FALSE;
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

	// Compile Tessellation Control Shader
	printf("Compiling shader : %s\n", tess_control_file_path);
	char const* TessControlSourcePointer = TessControlShaderCode.c_str();
	glShaderSource(TessControlShaderID, 1, &TessControlSourcePointer, NULL);
	glCompileShader(TessControlShaderID);

	// Check Tessellation Control Shader
	glGetShaderiv(TessControlShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(TessControlShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> TessControlShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(TessControlShaderID, InfoLogLength, NULL, &TessControlShaderErrorMessage[0]);
		printf("%s\n", &TessControlShaderErrorMessage[0]);
	}

	// Compile Tessellation Evaluation Shader
	printf("Compiling shader : %s\n", tess_evaluation_file_path);
	char const* TessEvaSourcePointer = TessEvaShaderCode.c_str();
	glShaderSource(TessEvaluationShaderID, 1, &TessEvaSourcePointer, NULL);
	glCompileShader(TessEvaluationShaderID);

	// Check Tessellation Evaluation Shader
	glGetShaderiv(TessEvaluationShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(TessEvaluationShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> TessEvaShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(TessEvaluationShaderID, InfoLogLength, NULL, &TessEvaShaderErrorMessage[0]);
		printf("%s\n", &TessEvaShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	//if (geom_file_path) {

	//	// Compile Geometry Shader
	//	printf("Compiling shader : %s\n", geom_file_path);
	//	char const* FragmentSourcePointer = GeomShaderCode.c_str();
	//	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	//	glCompileShader(FragmentShaderID);

	//	// Check Geometry Shader
	//	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	//	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	//	if (InfoLogLength > 0) {
	//		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
	//		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
	//		printf("%s\n", &FragmentShaderErrorMessage[0]);
	//	}
	//}



	// Link the program
	printf("Linking program\n");
	unsigned int ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, TessControlShaderID);
	glAttachShader(ProgramID, TessEvaluationShaderID);
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
	glDetachShader(ProgramID, TessControlShaderID);
	glDetachShader(ProgramID, TessEvaluationShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(TessControlShaderID);
	glDeleteShader(TessEvaluationShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}