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

	unsigned int mID;
	unsigned int vID;
	unsigned int pID;

	unsigned int albedoColorID;
	unsigned int albedoTextureID;
	unsigned int normalTextureID;
	unsigned int metallicTextureID;
	unsigned int roughnessTextureID;
	unsigned int aoTextureID;

	unsigned int normalAmountID;
	unsigned int metallicAmountID;
	unsigned int roughnessAmountID;
	unsigned int aoAmountID;

	unsigned int camPosID;

	Material();

	GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);

	void compileShaders();

	void loadUniforms();

	void deleteProgram();
};