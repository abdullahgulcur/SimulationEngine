#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

enum class MaterialType { pbr, phong };

class Material {

public:

	int fileID;
	std::string name;
	MaterialType type;
	bool useAlbedo = false;
	bool useNormal = false;
	bool useMetallic = false;
	bool useRoughness = false;
	bool useAO = false;
	std::string albedoTexturePath;
	std::string normalTexturePath;
	std::string metallicTexturePath;
	std::string roughnessTexturePath;
	std::string aoTexturePath;
	unsigned int dirLightCount = 0;
	unsigned int pointLightCount = 0;

	glm::vec3 albedoColor;
	unsigned int albedoTexture;
	unsigned int normalTexture;
	unsigned int metallicTexture;
	unsigned int roughnessTexture;
	unsigned int aoTexture;

	float normalAmount = 0.0f;
	float metallicAmount = 0.0f;
	float roughnessAmount = 0.5f;
	float aoAmount = 0.0f;

	unsigned int programID;

	Material();

	GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);

	void compileShaders();

	void deleteProgram();
};