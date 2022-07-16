#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/intersect.hpp>
#include <vector>
#include <math.h>
#include <basetsd.h>

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
#define LEVEL 5


class SceneCamera;
class Editor;

using namespace Material;

enum class BrushType { brush, texture, tree, grass };

struct HeightAndGradient {

	float height;
	float gradientX;
	float gradientZ;
};

class TerrainGenerator : public Component {

private:

public:

	BrushType brushType;

	unsigned short clipmapResolution = 16;
	unsigned short clipmapResolution_temp = 16;

	unsigned short clipmapLevel = 5;

	unsigned int programID;
	unsigned int elevationMapTexture;
	unsigned int colorMapTexture;
	unsigned int normalMapTexture;

	unsigned int grass_a_tex;
	unsigned int grass_ao_tex;
	unsigned int grass_n_tex;
	unsigned int grass_r_tex;

	std::vector<unsigned int> albedoMaps;
	std::vector<unsigned int> normalMaps;

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

	unsigned int elevationMapSize = 2048;

	glm::vec3* brushIntersectionPoint = NULL;
	bool canUseBrush = false;

	// Gizmos
	unsigned int gizmoVAO;
	unsigned int gizmoShaderProgramID;
	float brushRadius = 15.f;
	float brushIntensity = 50.f;

	//float* heights;
	float** heightMipMaps;
	unsigned char* colors;
	char* normals;

	float GKernel[GFILTER_SIZE][GFILTER_SIZE];

	// textures
	unsigned int texturePaletteIndex = 0;
	std::vector<unsigned short> paletteIndices;

	// properties
	std::vector<glm::vec2> slopeFilters;
	std::vector<glm::vec2> heightFilters;
	unsigned int activeSlopes;
	unsigned int activeHeights;

	// perlin noise
	unsigned int perlinSeed = 6428;
	unsigned char perlinOctaves = 3;
	float perlinScale = 0.005f;
	float heightScale = 100.f;
	float perlinPersistence = 0.5f;


	TerrainGenerator();

	glm::vec3* findIntersectionPoint(glm::vec3 upperPoint, glm::vec3 lowerPoint, int counter);

	void init();

	void init(MaterialFile* mat);

	//float* getFlatHeightmap(int size);

	float** getFlatHeightmap();

	unsigned char* getColorMap(int size);

	char* getNormalMap(int size);

	//float* getElevationData(int size);

	void addNewPalette(unsigned int albedoTexture, unsigned int normalTexture);

	void removePalette(int index);

	void selectAllTexturesFromPalette(int index);

	void disableAllTexturesFromPalette(int index);

	void brushControls(float mousePosX, float mousePosY, SceneCamera* camera);

	void drawBrush(glm::mat4 MVP);

	void setBrushRadius(float step);

	void setBrushIntensity(float step);

	void setSlopeFilters(int size);

	void setHeightFilters(int size);

	void setPerlinOctaves(int size);

	void setClipmapResolution(int resolution);

	void setClipmapLevel(int level);

	void setBoundariesOfClipmap(const int& level, glm::vec3& start, glm::vec3& end);

	void applyPerlinNoise();

	void gaussianFilterCreation();

	void bump();

	void paint();

	float angleBetweenTwoVectors(glm::vec3 v0, glm::vec3 v1);

	void grass();

	void tree();

	void recalculateNormal(int x, int z);

	void recreateHeightMap();

	void recreateColorMap();

	void recreateNormalMap();

	void createNormalMap();

	void createColorMap();

	void createHeightMap();

	void getMaxAndMinHeights(glm::vec2& bounds, const int& level, const glm::vec3& start, const glm::vec3& end);

	void drawTerrainClipmapAABB(const glm::vec3& start, const glm::vec3& end, const SceneCamera* camera, const Editor* editor);

	void drawInScene(SceneCamera* camera, GameCamera* gc, Editor* editor);

	void drawInGame(GameCamera* camera);

	void applyFiltersInWholeMap();

	void generateTerrainClipmapsVertexArrays();

	void applyClipmapProperties();

	void createBrushGizmo();

};