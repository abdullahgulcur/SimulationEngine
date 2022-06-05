#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/intersect.hpp>
#include <vector>
#include <math.h>

#include "shader.hpp"
#include "component.hpp"
#include "material.hpp"
#include "transform.hpp"
#include "texture.hpp"
#include "utility.hpp"
#include "math.hpp"
#include "gamecamera.hpp"

#include "perlinnoise/perlinnoise.hpp"

#define PI 3.14159
#define BRUSH_RESOLUTION 32
#define GFILTER_SIZE 201


class SceneCamera;

using namespace Material;

struct HeightAndGradient {

	float height;
	float gradientX;
	float gradientZ;
};

class TerrainGenerator : public Component {

private:

public:

	int patchSize = 8;

	unsigned int programID;
	unsigned int elevationMapTexture;

	unsigned int grass_a_tex;
	unsigned int grass_ao_tex;
	unsigned int grass_n_tex;
	unsigned int grass_r_tex;

	std::vector<unsigned int> blockIndices;
	unsigned int blockVAO;

	unsigned int ringFixUpHorizontalVAO;
	std::vector<unsigned int> ringFixUpHorizontalIndices;

	unsigned int ringFixUpVerticalVAO;
	std::vector<unsigned int> ringFixUpVerticalIndices;

	unsigned int smallSquareVAO;
	std::vector<unsigned int> smallSquareIndices;

	unsigned int outerDegenerateVAO;
	std::vector<unsigned int> outerDegenerateIndices;

	MaterialFile* mat;
	File* heightmap;
	unsigned int heighmapTextureID;

	int elevationMapSize = 2048;

	glm::vec3* brushIntersectionPoint = NULL;
	bool canUseBrush = false;

	// Gizmos
	unsigned int gizmoVAO;
	unsigned int gizmoShaderProgramID;
	float brushRadius = 15.f;
	float brushIntensity = 50.f;
	//std::vector<float> heights;
	float* heights;

	float GKernel[GFILTER_SIZE][GFILTER_SIZE];

	TerrainGenerator();

	glm::vec3* findIntersectionPoint(glm::vec3 upperPoint, glm::vec3 lowerPoint, int counter);

	void init();

	void init(MaterialFile* mat);

	float* getFlatHeightmap(int size);

	float* getElevationData(int size);

	void brushControls(float mousePosX, float mousePosY, SceneCamera* camera);

	void drawBrush(glm::mat4 MVP);

	void setBrushRadius(float step);

	void setBrushIntensity(float step);

	void gaussianFilterCreation();

	void bump();

	void recreateHeightMap();

	void createHeightMap();

	void drawInScene(SceneCamera* camera, GameCamera* gc);

	void drawInGame(GameCamera* camera);

	void createBrushGizmo();

};