#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <math.h>

#include "shader.hpp"
#include "component.hpp"
#include "material.hpp"
#include "transform.hpp"
#include "texture.hpp"
#include "utility.hpp"
#include "math.hpp"

#include "perlinnoise/perlinnoise.hpp"

class Camera;

using namespace Material;

#define PI 3.1415

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

struct HeightAndGradient {

	float height;
	float gradientX;
	float gradientZ;
};

struct TerrainChunk {

	std::vector<std::vector<TerrainVertex>> lods;
};

class TerrainGenerator : public Component {

private:

	// 390150
	//int indiceCount;

public:

	const unsigned int NUM_PATCH_PTS = 4;
	unsigned rez = 20;

	unsigned int programID;
	unsigned int terrainVAO;
	unsigned int elevationMapTexture;

	unsigned int clipmaps = 8;

	std::vector<std::vector<unsigned int>> DebugNormalVAOs; // dim 0: clipmaps 1:lods
	std::vector<std::vector<unsigned int>> VAOs; // dim 0: clipmaps 1:lods
	//std::vector<TerrainChunk> chunks;
	MaterialFile* mat;
	File* heightmap;
	unsigned int seed = 12456u;
	unsigned int heighmapTextureID;
	//unsigned short viewportLevel_X = 10;
	//unsigned short viewportLevel_Z = 10;
	//float size_X = 1.f;
	//float size_Z = 1.f;
	float height = 10.f;
	float scale = 3.f;

	float minHeight;
	float maxHeight = 0.f;
	float averageHeight;

	int elevationMapSize = 1024;

	//// erosion params
	//std::vector<float> heights;
	//int numIterations = 70000;
	//float initialWaterVolume = 1;
	//float initialSpeed = 1;
	//int erosionRadius = 3;
	//float inertia = .05f; // At zero, water will instantly change direction to flow downhill. At 1, water will never change direction. 
	//float sedimentCapacityFactor = 4; // Multiplier for how much sediment a droplet can carry
	//float minSedimentCapacity = .01f; // Used to prevent carry capacity getting too close to zero on flatter terrain
	//float erodeSpeed = .3f;
	//float depositSpeed = .3f;
	//float evaporateSpeed = .01f;
	//float gravity = 4;
	//int maxDropletLifetime = 30;

	// erosion params
	std::vector<float> heights;
	

	// Indices and weights of erosion brush precomputed for every node
	std::vector<std::vector<int>> erosionBrushIndices;
	std::vector<std::vector<float>> erosionBrushWeights;


	// temp
	unsigned int debugNormalProgramID;

	TerrainGenerator();

	void init();

	void init(MaterialFile* mat);

	void recreateHeightField();

	void erode();

	void createHeightMap();

	void filterElevationMap(int size, int iterations);

	void scaleElevationMap(int scaleFactor);

	//void smoothFilterCreation(double** GKernel, int size);

	void gaussianFilterCreation(double** GKernel, int size);

	void createTerrainMesh(std::vector<TerrainChunk>& chunks, std::vector<std::vector<unsigned short>>& indices);

	HeightAndGradient calculateHeightAndGradient(float posX, float posZ);

	void initializeBrushIndices(int radius);

	glm::vec4 evaluateColor(float& slope);

	float angleBetweenTwoVectors(glm::vec3 v0, glm::vec3 v1);

	void recalculateNormals(std::vector<TerrainChunk>& chunks, std::vector<std::vector<unsigned short>>& indices);

	void initDebugNormalLines(std::vector<TerrainChunk>& chunks, std::vector<std::vector<unsigned short>>& indices);

	int getLODLevel(glm::vec2 camPos, int chunkIndex);

	void createHeightField(std::vector<TerrainChunk>& chunks, std::vector<std::vector<unsigned short>>& indices);

	void draw(Camera* camera, std::vector<Transform*>& pointLightTransforms, std::vector<Transform*>& dirLightTransforms);

};