#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

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

class Terrain : public Component {

private:

public:

	unsigned int VAO;
	MaterialFile* mat;
	unsigned int seed = 123456u;
	unsigned short viewportLevel_X = 10;
	unsigned short viewportLevel_Z = 10;
	int indiceCount;
	float size_X = 10.f;
	float size_Z = 10.f;
	float height = 1.f;
	float scale = 1.f;

	Terrain();

	void init(MaterialFile* mat);

	void recreateHeightField();

	void createTerrainMesh(std::vector<unsigned short>& indices, std::vector<TerrainVertex>& vertices);

	glm::vec4 evaluateColor(float& slope);

	float angleBetweenTwoVectors(glm::vec3 v0, glm::vec3 v1);

	void recalculateNormals(std::vector<unsigned short>& indices, std::vector<TerrainVertex>& vertices);

	void createHeightField(std::vector<unsigned short>& indices, std::vector<TerrainVertex>& vertices);

	void draw(Camera* camera, std::vector<Transform*>& pointLightTransforms, std::vector<Transform*>& dirLightTransforms);

};