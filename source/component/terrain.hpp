#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <math.h>

#include "shader.hpp"
#include "component.hpp"
#include "material.hpp"
#include "transform.hpp"

#include "perlinnoise/perlinnoise.hpp"

class Camera;

using namespace Material;
//
//struct vec8 {
//
//	float a;
//	float b;
//
//	vec8() {}
//
//	vec8(float a, float b) {
//
//		this->a = a;
//		this->b = b;
//	}
//};

struct TerrainVertex {

	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;
	glm::vec4 color;

	TerrainVertex() {}

	TerrainVertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texCoord, glm::vec4 color) {

		this->position = position;
		this->normal = normal;
		this->texCoord = texCoord;
		this->color = color;
	}
};

struct TerrainChunk {

	std::vector<std::vector<TerrainVertex>> lods;
};

class TerrainGenerator : public Component {

private:

	// 390150
	//int indiceCount;

public:

	unsigned int clipmaps = 8;

	std::vector<std::vector<unsigned int>> DebugNormalVAOs; // dim 0: clipmaps 1:lods
	std::vector<std::vector<unsigned int>> VAOs; // dim 0: clipmaps 1:lods
	//std::vector<TerrainChunk> chunks;
	MaterialFile* mat;
	unsigned int seed = 123456u;
	unsigned short viewportLevel_X = 10;
	unsigned short viewportLevel_Z = 10;
	float size_X = 1.f;
	float size_Z = 1.f;
	float height = 10.f;
	float scale = 3.f;

	// temp
	unsigned int debugNormalProgramID;

	TerrainGenerator();

	void init();

	void init(MaterialFile* mat);

	void recreateHeightField();

	void createTerrainMesh(std::vector<TerrainChunk>& chunks, std::vector<std::vector<unsigned short>>& indices);

	glm::vec4 evaluateColor(float& slope);

	float angleBetweenTwoVectors(glm::vec3 v0, glm::vec3 v1);

	void recalculateNormals(std::vector<TerrainChunk>& chunks, std::vector<std::vector<unsigned short>>& indices);

	void initDebugNormalLines(std::vector<TerrainChunk>& chunks, std::vector<std::vector<unsigned short>>& indices);

	int getLODLevel(glm::vec3 camPos, int chunkIndex);

	void createHeightField(std::vector<TerrainChunk>& chunks, std::vector<std::vector<unsigned short>>& indices);

	void draw(Camera* camera, std::vector<Transform*>& pointLightTransforms, std::vector<Transform*>& dirLightTransforms);

};