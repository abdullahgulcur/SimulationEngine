#pragma once

#include <sstream>
#include <fstream>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

enum class MaterialType { pbr, phong };

struct Mat {

};

class Material {

public:

	MaterialType type;

	bool useAlbedo = false;
	bool useNormal = false;
	bool useMetallic = false;
	bool useRoughness = false;
	bool useAO = false;

	float normalAmount = 0.0f;
	float metallicAmount = 0.0f;
	float roughnessAmount = 0.5f;
	glm::vec3 albedoColor;

	std::string albedoTexturePath;
	std::string normalTexturePath;
	std::string metallicTexturePath;
	std::string roughnessTexturePath;
	std::string aoTexturePath;

	unsigned int albedoTextureID;
	unsigned int normalTextureID;
	unsigned int metallicTextureID;
	unsigned int roughnessTextureID;
	unsigned int aoTextureID;

	Material();

	GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);

	void loadMaterial(const char* mat_file_path);
};