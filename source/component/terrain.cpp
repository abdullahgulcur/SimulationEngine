#include "terrain.hpp"
#include "scenecamera.hpp"

#define ASSERT(x) if(!(x)) __debugbreak;
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void GLClearError() {

	while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line) {

	while (GLenum error = glGetError()) {

		std::cout << "[OpenGL Error] (" << error << "): " << function << " " << file << ":" << line << std::endl;
		return false;
	}
	return true;
}

TerrainGenerator::TerrainGenerator() {

	TerrainGenerator::createBrushGizmo();
	TerrainGenerator::gaussianFilterCreation();

}

void TerrainGenerator::init() {

	TerrainGenerator::createHeightMap();
}

void TerrainGenerator::init(MaterialFile* mat) {

	TerrainGenerator::createHeightMap();

}

float* TerrainGenerator::getFlatHeightmap(int size) {

	float* data = new float[size * size];

	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
			data[i * size + j] = 0;

	return data;
}

//float* TerrainGenerator::getElevationData(int size) {
//
//	//const siv::PerlinNoise perlin(Utility::getRandomInt(0, 99999));
//	const siv::PerlinNoise perlin(999);
//
//	float* data = new float[size * size];
//
//	for (int i = 0; i < size; i++) {
//
//		for (int j = 0; j < size; j++) {
//
//			//float height = perlin.octave2D_01(i * 0.002f, j * 0.002f, 3) * 130.f;
//			//height = height < 40.f ? height = 40.f - ((40.f - height) * 0.8f) : height;
//			//height = height < 35.f ? height = 35.f - ((35.f - height) * 0.8f) : height;
//			//height = height < 30.f ? height = 30.f - ((30.f - height) * 0.8f) : height;
//			float height = heights[i * size + j];
//			//float height = 0;
//			data[i * size + j] = height;
//
//			//heights.push_back(height);
//		}
//	}
//
//	return data;
//}

float* TerrainGenerator::getElevationData(int size) {

	float* data = new float[size * size];

	for (int i = 0; i < size; i++) {

		for (int j = 0; j < size; j++) {

			float height = heights[i * size + j];
			data[i * size + j] = height;
		}
	}

	return data;
}

// ref: https://antongerdelan.net/opengl/raycasting.html
void TerrainGenerator::brushControls(float mousePosX, float mousePosY, SceneCamera* camera) {

	float minHeight = -100;
	delete brushIntersectionPoint;

	if (camera->position.y < 0) {

		brushIntersectionPoint = NULL;
		canUseBrush = false;
		return;
	}

	glm::vec3 ray_nds = glm::vec3(mousePosX, mousePosY, 1.f);
	glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.f, 1.f);
	glm::vec4 ray_eye = glm::inverse(camera->ProjectionMatrix) * ray_clip;
	ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.f, 0.f);
	glm::vec4 t = glm::inverse(camera->ViewMatrix) * ray_eye;
	glm::vec3 ray_world = glm::vec3(t.x, t.y, t.z);
	ray_world = glm::normalize(ray_world);
	//std::cout << "World Ray cast x: " << ray_world.x << " y: " << ray_world.y << " z: " << ray_world.x << std::endl;
	glm::vec3 planeOrig(0, minHeight, 0);
	glm::vec3 planeNormal(0, 1, 0);
	float intersectionDistance;
	glm::intersectRayPlane(camera->position, ray_world, planeOrig, planeNormal, intersectionDistance);
	glm::vec3 intersectionPoint = camera->position + ray_world * intersectionDistance;
	//std::cout << "Intersection: " << intersectionPoint.x << " y: " << intersectionPoint.y << " z: " << intersectionPoint.z << std::endl;
	glm::vec3* intersection = findIntersectionPoint(camera->position, intersectionPoint, 0);

	if (!intersection) {

		brushIntersectionPoint = NULL;
		canUseBrush = false;
		return;
	}

	// -------- Brush gizmo ---------
	glm::mat4 brushAreaModel(1.f);
	glm::mat4 brushIntensityCircleModel(1.f);
	intersection->y += 1;

	if (intersection->x > elevationMapSize || intersection->x < 0 ||
		intersection->z > elevationMapSize || intersection->z < 0) {

		brushIntersectionPoint = NULL;
	}
	else {

		float maxBrushParam = 100.f;
		brushAreaModel = glm::translate(brushAreaModel, *intersection) * glm::scale(glm::mat4(1.0f), glm::vec3(brushRadius / 2));
		brushIntensityCircleModel = glm::translate(brushIntensityCircleModel, *intersection) * glm::scale(glm::mat4(1.0f), glm::vec3(brushRadius * brushIntensity * 0.9f / (maxBrushParam * 2)));

		glm::mat4 mvp = camera->ProjectionMatrix * camera->ViewMatrix * brushAreaModel;
		TerrainGenerator::drawBrush(mvp);

		glm::mat4 brushIntensityCircleMVP = camera->ProjectionMatrix * camera->ViewMatrix * brushIntensityCircleModel;
		TerrainGenerator::drawBrush(brushIntensityCircleMVP);

		brushIntersectionPoint = new glm::vec3(*intersection);
		canUseBrush = true;
	}

	delete intersection;
}

glm::vec3* TerrainGenerator::findIntersectionPoint(glm::vec3 upperPoint, glm::vec3 lowerPoint, int counter) {

	glm::vec3 middle = (upperPoint + lowerPoint) / (float)2;

	if (counter == 1000)
		return new glm::vec3(middle);

	int heightIndex = (int)middle.z * elevationMapSize + (int)middle.x;

	if (heightIndex >= elevationMapSize * elevationMapSize || heightIndex < 0)
		return NULL;

	float elevation = heights[heightIndex];

	if (abs(middle.y - elevation) < 1.f)
		return new glm::vec3(middle);

	if (elevation > middle.y)
		return TerrainGenerator::findIntersectionPoint(upperPoint, middle, ++counter);
	else
		return TerrainGenerator::findIntersectionPoint(middle, lowerPoint, ++counter);
}


void TerrainGenerator::drawBrush(glm::mat4 MVP) {

	glUseProgram(gizmoShaderProgramID);
	glUniformMatrix4fv(glGetUniformLocation(gizmoShaderProgramID, "MVP"), 1, GL_FALSE, &MVP[0][0]);
	glUniform3fv(glGetUniformLocation(gizmoShaderProgramID, "color"), 1, &glm::vec3(0, 1, 0)[0]);

	glBindVertexArray(gizmoVAO);
	glDrawArrays(GL_LINE_STRIP, 0, BRUSH_RESOLUTION + 1);
	glBindVertexArray(0);
}

void TerrainGenerator::gaussianFilterCreation() {
	// initialising standard deviation to 1.0
	float sigma = GFILTER_SIZE / 8;
	double r, s = 2.0 * sigma * sigma;

	// sum is for normalization
	double sum = 0.0;

	// generating mxm kernel
	for (int x = -GFILTER_SIZE / 2; x <= GFILTER_SIZE / 2; x++) {
		for (int y = -GFILTER_SIZE / 2; y <= GFILTER_SIZE / 2; y++) {
			r = sqrt(x * x + y * y);
			GKernel[x + GFILTER_SIZE / 2][y + GFILTER_SIZE / 2] = (exp(-(r * r) / s)) / (PI * s);
			sum += GKernel[x + GFILTER_SIZE / 2][y + GFILTER_SIZE / 2];
		}
	}

	// normalising the Kernel
	for (int i = 0; i < GFILTER_SIZE; ++i)
		for (int j = 0; j < GFILTER_SIZE; ++j)
			GKernel[i][j] /= sum;
}

void TerrainGenerator::bump() {

	for (int i = -brushRadius; i < brushRadius; i++) {

		for (int j = -brushRadius; j < brushRadius; j++) {

			glm::vec3 posInTex(brushIntersectionPoint->x + i, 0, brushIntersectionPoint->z + j);

			int x = i + brushRadius;
			int z = j + brushRadius;

			int _x = x * (float)GFILTER_SIZE / (brushRadius * 2);
			int _z = z * (float)GFILTER_SIZE / (brushRadius * 2);

			float elevation = GKernel[_x][_z] * 50 * brushIntensity;

			int index = (int)posInTex.z * elevationMapSize + (int)posInTex.x;

			if(index > 0 && index < elevationMapSize * elevationMapSize)
				heights[index] += elevation;
		}
	}

	TerrainGenerator::recreateHeightMap();
}

void TerrainGenerator::recreateHeightMap() {

	glDeleteTextures(1, &elevationMapTexture);

	GLint Alignment = 0;
	glGetIntegerv(GL_UNPACK_ALIGNMENT, &Alignment);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &elevationMapTexture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, elevationMapTexture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	unsigned int width = elevationMapSize;
	unsigned int height = elevationMapSize;
	//float* data = TerrainGenerator::getElevationData(width);
	elevationMapSize = height;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, heights);

	glBindTexture(GL_TEXTURE_2D, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, Alignment);

	glGenerateMipmap(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(programID, "heightMap"), 0);
	//delete[] data;
}

void TerrainGenerator::createHeightMap() {

	GLint Alignment = 0;
	glGetIntegerv(GL_UNPACK_ALIGNMENT, &Alignment);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &elevationMapTexture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, elevationMapTexture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	unsigned int width = elevationMapSize;
	unsigned int height = elevationMapSize;
	//unsigned char* data = TextureNS::loadPNG("resource/heightmaps/height.png", width, height);
	heights = TerrainGenerator::getFlatHeightmap(width);
	//unsigned char* data = TextureNS::loadPNG("resource/heightmaps/elevationmap.png", height, height);
	elevationMapSize = height;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, heights);

	glBindTexture(GL_TEXTURE_2D, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, Alignment);

	glGenerateMipmap(GL_TEXTURE_2D);
	glUniform1i(glGetUniformLocation(programID, "heightMap"), 0);
	//delete[] data;

	//surfaceTexture = TextureNS::loadDDS("resource/textures/surface.DDS");
	grass_a_tex = TextureNS::loadDDS("resource/textures/grass_a.DDS");
	grass_ao_tex = TextureNS::loadDDS("resource/textures/grass_ao.DDS");
	grass_n_tex = TextureNS::loadDDS("resource/textures/grass_n.DDS");
	grass_r_tex = TextureNS::loadDDS("resource/textures/grass_r.DDS");
	// 
	//elevationMapTexture = TextureNS::loadDDS("resource/heightmaps/mountain_big.DDS");

	programID = ShaderNS::loadShaders("source/shader/clipmap.vert", "source/shader/clipmap.frag");

	std::vector<glm::vec3> verts;
	std::vector<glm::vec3> ringFixUpVerts;
	std::vector<glm::vec3> ringFixUpVerts1;
	std::vector<glm::vec3> smallSquareVerts;
	std::vector<glm::vec3> outerDegenerateVerts;


	for (int i = 0; i < patchSize; i++)
		for (int j = 0; j < patchSize; j++)
			verts.push_back(glm::vec3(j, 0, i));

	for (int i = 0; i < patchSize - 1; i++) {

		for (int j = 0; j < patchSize - 1; j++) {

			blockIndices.push_back(j + i * (patchSize));
			blockIndices.push_back(j + (i + 1) * (patchSize));
			blockIndices.push_back(j + i * (patchSize) + 1);

			blockIndices.push_back(j + i * (patchSize) + 1);
			blockIndices.push_back(j + (i + 1) * (patchSize));
			blockIndices.push_back(j + (i + 1) * (patchSize) + 1);
		}
	}

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			smallSquareVerts.push_back(glm::vec3(j, 0, i));

	for (int i = 0; i < 3 - 1; i++) {

		for (int j = 0; j < 3 - 1; j++) {

			smallSquareIndices.push_back(j + i * (3));
			smallSquareIndices.push_back(j + (i + 1) * (3));
			smallSquareIndices.push_back(j + i * (3)+1);

			smallSquareIndices.push_back(j + i * (3)+1);
			smallSquareIndices.push_back(j + (i + 1) * (3));
			smallSquareIndices.push_back(j + (i + 1) * (3)+1);
		}
	}

	for (int i = 0; i < patchSize; i++)
		for (int j = 0; j < 3; j++)
			ringFixUpVerts.push_back(glm::vec3(j, 0, i));

	for (int i = 0; i < patchSize - 1; i++) {

		for (int j = 0; j < 3 - 1; j++) {

			ringFixUpHorizontalIndices.push_back(j + i * (3));
			ringFixUpHorizontalIndices.push_back(j + (i + 1) * (3));
			ringFixUpHorizontalIndices.push_back(j + i * (3)+1);

			ringFixUpHorizontalIndices.push_back(j + i * (3)+1);
			ringFixUpHorizontalIndices.push_back(j + (i + 1) * (3));
			ringFixUpHorizontalIndices.push_back(j + (i + 1) * (3)+1);
		}
	}

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < patchSize; j++)
			ringFixUpVerts1.push_back(glm::vec3(j, 0, i));

	for (int i = 0; i < 3 - 1; i++) {

		for (int j = 0; j < patchSize - 1; j++) {

			ringFixUpVerticalIndices.push_back(j + i * (patchSize));
			ringFixUpVerticalIndices.push_back(j + (i + 1) * (patchSize));
			ringFixUpVerticalIndices.push_back(j + i * (patchSize) + 1);
							
			ringFixUpVerticalIndices.push_back(j + i * (patchSize) + 1);
			ringFixUpVerticalIndices.push_back(j + (i + 1) * (patchSize));
			ringFixUpVerticalIndices.push_back(j + (i + 1) * (patchSize) + 1);
		}
	}

	// Outer degenerate triangles (y lere hic gerek yok !!!)
	for (int i = 0; i < patchSize * 4; i += 2) {

		outerDegenerateVerts.push_back(glm::vec3(0, 0, i));
		outerDegenerateVerts.push_back(glm::vec3(0, 0, i + 1));
		outerDegenerateVerts.push_back(glm::vec3(0, 0, i + 2));
	}

	for (int i = 0; i < patchSize * 4; i += 2) {

		outerDegenerateVerts.push_back(glm::vec3(i, 0, patchSize * 4));
		outerDegenerateVerts.push_back(glm::vec3(i + 1, 0, patchSize * 4));
		outerDegenerateVerts.push_back(glm::vec3(i + 2, 0, patchSize * 4));
	}

	for (int i = patchSize * 4; i > 0; i -= 2) {

		outerDegenerateVerts.push_back(glm::vec3(patchSize * 4, 0, i));
		outerDegenerateVerts.push_back(glm::vec3(patchSize * 4, 0, i - 1));
		outerDegenerateVerts.push_back(glm::vec3(patchSize * 4, 0, i - 2));
	}

	for (int i = patchSize * 4; i > 0; i -= 2) {

		outerDegenerateVerts.push_back(glm::vec3(i, 0, 0));
		outerDegenerateVerts.push_back(glm::vec3(i - 1, 0, 0));
		outerDegenerateVerts.push_back(glm::vec3(i - 2, 0, 0));
	}

	for (int i = 0; i < patchSize * 3 * 2 * 4; i++)
		outerDegenerateIndices.push_back(i);

	for (int i = patchSize * 3 * 2 * 4 - 1; i >= 0; i--)
		outerDegenerateIndices.push_back(i);

	// Block
	glGenVertexArrays(1, &blockVAO);
	glBindVertexArray(blockVAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(glm::vec3), &verts[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, blockIndices.size() * sizeof(unsigned int), &blockIndices[0], GL_STATIC_DRAW);

	// Ring Fix-up (Horizontal)
	glGenVertexArrays(1, &ringFixUpHorizontalVAO);
	glBindVertexArray(ringFixUpHorizontalVAO);

	unsigned int ringFixUpVBO;
	glGenBuffers(1, &ringFixUpVBO);
	glBindBuffer(GL_ARRAY_BUFFER, ringFixUpVBO);
	glBufferData(GL_ARRAY_BUFFER, ringFixUpVerts.size() * sizeof(glm::vec3), &ringFixUpVerts[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

	unsigned int ringFixUpEBO;
	glGenBuffers(1, &ringFixUpEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ringFixUpEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ringFixUpHorizontalIndices.size() * sizeof(unsigned int), &ringFixUpHorizontalIndices[0], GL_STATIC_DRAW);

	// Ring Fix-up (Vertical)
	glGenVertexArrays(1, &ringFixUpVerticalVAO);
	glBindVertexArray(ringFixUpVerticalVAO);

	unsigned int ringFixUpVBO1;
	glGenBuffers(1, &ringFixUpVBO1);
	glBindBuffer(GL_ARRAY_BUFFER, ringFixUpVBO1);
	glBufferData(GL_ARRAY_BUFFER, ringFixUpVerts1.size() * sizeof(glm::vec3), &ringFixUpVerts1[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

	unsigned int ringFixUpEBO1;
	glGenBuffers(1, &ringFixUpEBO1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ringFixUpEBO1);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ringFixUpVerticalIndices.size() * sizeof(unsigned int), &ringFixUpVerticalIndices[0], GL_STATIC_DRAW);

	// Small Square
	glGenVertexArrays(1, &smallSquareVAO);
	glBindVertexArray(smallSquareVAO);

	unsigned int smallSquareVBO;
	glGenBuffers(1, &smallSquareVBO);
	glBindBuffer(GL_ARRAY_BUFFER, smallSquareVBO);
	glBufferData(GL_ARRAY_BUFFER, smallSquareVerts.size() * sizeof(glm::vec3), &smallSquareVerts[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

	unsigned int smallSquareEBO;
	glGenBuffers(1, &smallSquareEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, smallSquareEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, smallSquareIndices.size() * sizeof(unsigned int), &smallSquareIndices[0], GL_STATIC_DRAW);

	// Outer Degenerate
	glGenVertexArrays(1, &outerDegenerateVAO);
	glBindVertexArray(outerDegenerateVAO);

	unsigned int outerDegenerateVBO;
	glGenBuffers(1, &outerDegenerateVBO);
	glBindBuffer(GL_ARRAY_BUFFER, outerDegenerateVBO);
	glBufferData(GL_ARRAY_BUFFER, outerDegenerateVerts.size() * sizeof(glm::vec3), &outerDegenerateVerts[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

	unsigned int outerDegenerateEBO;
	glGenBuffers(1, &outerDegenerateEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, outerDegenerateEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, outerDegenerateIndices.size() * sizeof(unsigned int), &outerDegenerateIndices[0], GL_STATIC_DRAW);
}

void TerrainGenerator::drawInScene(SceneCamera* camera, GameCamera* gc) {

	int count = 0;
	int scale = 1;
	int level = 6;
	int worldSize = patchSize * 2 * (2 << (level - 1));

	GLCall(glUseProgram(programID));

	glUniformMatrix4fv(glGetUniformLocation(programID, "V"), 1, GL_FALSE, &camera->ViewMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(programID, "P"), 1, GL_FALSE, &camera->ProjectionMatrix[0][0]);
	glUniform1i(glGetUniformLocation(programID, "patchRes"), patchSize);
	glUniform1i(glGetUniformLocation(programID, "mapSize"), elevationMapSize);
	glUniform1i(glGetUniformLocation(programID, "clipMapSize"), worldSize);
	GLCall(glUniform3fv(glGetUniformLocation(programID, "camPos"), 1, &camera->position[0]));

	glm::vec3 lightDir = glm::normalize(-glm::vec3(1, -1, -1));
	GLCall(glUniform3fv(glGetUniformLocation(programID, "lightDir"), 1, &lightDir[0]));
	//GLCall(glUniform3fv(glGetUniformLocation(programID, "dirLightColor"), 1, &glm::vec3(1, 1, 1)[0]));
	//GLCall(glUniform1f(glGetUniformLocation(programID, "dirLightPower"), 100));

	//GLCall(glUniform3fv(glGetUniformLocation(programID, "lightPosition"), 1, &glm::vec3(pos, 10, pos)[0]));
	GLCall(glUniform3fv(glGetUniformLocation(programID, "lightColor"), 1, &glm::vec3(1, 1, 1)[0]));

	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, elevationMapTexture));
	GLCall(glUniform1i(glGetUniformLocation(programID, "heightmap"), 0));


	GLCall(glActiveTexture(GL_TEXTURE1));
	GLCall(glBindTexture(GL_TEXTURE_2D, grass_a_tex));
	GLCall(glUniform1i(glGetUniformLocation(programID, "grass_a_tex"), 1));

	GLCall(glActiveTexture(GL_TEXTURE2));
	GLCall(glBindTexture(GL_TEXTURE_2D, grass_ao_tex));
	GLCall(glUniform1i(glGetUniformLocation(programID, "grass_ao_tex"), 2));

	GLCall(glActiveTexture(GL_TEXTURE3));
	GLCall(glBindTexture(GL_TEXTURE_2D, grass_n_tex));
	GLCall(glUniform1i(glGetUniformLocation(programID, "grass_n_tex"), 3));

	GLCall(glActiveTexture(GL_TEXTURE4));
	GLCall(glBindTexture(GL_TEXTURE_2D, grass_r_tex));
	GLCall(glUniform1i(glGetUniformLocation(programID, "grass_r_tex"), 4));

	for (int i = 0; i < level; i++) {

		float ox = int((camera->position.x / 2) * (1 / (float)(1 << i))) * (2 / (float)(patchSize));
		float oz = int((camera->position.z / 2) * (1 / (float)(1 << i))) * (2 / (float)(patchSize));
		float x = -((int)(ox * 4) % 2) * 4;
		float z = -((int)(oz * 4) % 2) * 4;
		float ax = -((int)(ox * 4) % 2) * (2 / (float)patchSize);
		float az = -((int)(oz * 4) % 2) * (2 / (float)patchSize);

		for (int j = -2; j < 2; j++) {

			for (int k = -2; k < 2; k++) {

				if (i != 0) if (k == -1 || k == 0) if (j == -1 || j == 0) continue;

				glm::vec3 blockOffset(j, 0, k);
				if (j == -1 || j == 1) blockOffset.x -= (float)1 / patchSize;
				if (k == -1 || k == 1) blockOffset.z -= (float)1 / patchSize;

				blockOffset.x += (float)2 / patchSize;
				blockOffset.z += (float)2 / patchSize;

				glm::vec3 off(blockOffset.x + ox, 0, blockOffset.z + oz);
				glUniform1i(glGetUniformLocation(programID, "scale"), scale);
				glUniform3fv(glGetUniformLocation(programID, "offset"), 1, &off[0]);
				glUniform3fv(glGetUniformLocation(programID, "color"), 1, &glm::vec3(0, 0, 1)[0]);
				glBindVertexArray(blockVAO);
				GLCall(glDrawElements(GL_TRIANGLES, blockIndices.size(), GL_UNSIGNED_INT, 0));
				count++;

				if (j == 0) {

					off = glm::vec3(blockOffset.x - (float)2 / patchSize + ox, 0, blockOffset.z + oz);
					glUniform3fv(glGetUniformLocation(programID, "offset"), 1, &off[0]);
					glUniform3fv(glGetUniformLocation(programID, "color"), 1, &glm::vec3(0, 1, 0)[0]);
					glBindVertexArray(ringFixUpHorizontalVAO);
					glDrawElements(GL_TRIANGLES, ringFixUpHorizontalIndices.size(), GL_UNSIGNED_INT, 0);
					count++;

					if (i == 0 && k == 0) {

						off.z -= (float)2 / patchSize;
						glUniform3fv(glGetUniformLocation(programID, "offset"), 1, &off[0]);
						glBindVertexArray(smallSquareVAO);
						glDrawElements(GL_TRIANGLES, smallSquareIndices.size(), GL_UNSIGNED_INT, 0);
						count++;
					}
				}
				else if (j == 1) {

					off = glm::vec3(blockOffset.x + 1 - 1 / (float)patchSize + ox + x, 0, blockOffset.z + oz);
					glUniform3fv(glGetUniformLocation(programID, "offset"), 1, &off[0]);
					glUniform3fv(glGetUniformLocation(programID, "color"), 1, &glm::vec3(0, 1, 1)[0]);
					glBindVertexArray(ringFixUpHorizontalVAO);
					glDrawElements(GL_TRIANGLES, ringFixUpHorizontalIndices.size(), GL_UNSIGNED_INT, 0);
					count++;

					if (k == 0) {

						off.z -= (float)2 / patchSize;
						glUniform3fv(glGetUniformLocation(programID, "offset"), 1, &off[0]);
						glBindVertexArray(smallSquareVAO);
						glDrawElements(GL_TRIANGLES, smallSquareIndices.size(), GL_UNSIGNED_INT, 0);
						count++;
					}
				}

				if (k == 1) {

					off = glm::vec3(blockOffset.x + ox, 0, blockOffset.z + 1 - 1 / (float)patchSize + oz + z);
					glUniform3fv(glGetUniformLocation(programID, "offset"), 1, &off[0]);
					glUniform3fv(glGetUniformLocation(programID, "color"), 1, &glm::vec3(0, 1, 1)[0]);
					glBindVertexArray(ringFixUpVerticalVAO);
					glDrawElements(GL_TRIANGLES, ringFixUpVerticalIndices.size(), GL_UNSIGNED_INT, 0);
					count++;

					if (j == 0) {

						off.x -= (float)2 / patchSize;
						glUniform3fv(glGetUniformLocation(programID, "offset"), 1, &off[0]);
						glBindVertexArray(smallSquareVAO);
						glDrawElements(GL_TRIANGLES, smallSquareIndices.size(), GL_UNSIGNED_INT, 0);
						count++;
					}
				}
				else if (k == 0) {

					off = glm::vec3(blockOffset.x + ox, 0, blockOffset.z - (float)2 / patchSize + oz);
					glUniform3fv(glGetUniformLocation(programID, "offset"), 1, &off[0]);
					glUniform3fv(glGetUniformLocation(programID, "color"), 1, &glm::vec3(1, 0.5, 0)[0]);
					glBindVertexArray(ringFixUpVerticalVAO);
					glDrawElements(GL_TRIANGLES, ringFixUpVerticalIndices.size(), GL_UNSIGNED_INT, 0);
					count++;
				}

				if (j == 1 && k == 1) {

					off = glm::vec3(blockOffset.x + 1 - 1 / (float)patchSize + ox + x, 0, blockOffset.z + 1 - 1 / (float)patchSize + oz + z);
					glUniform3fv(glGetUniformLocation(programID, "offset"), 1, &off[0]);
					glUniform3fv(glGetUniformLocation(programID, "color"), 1, &glm::vec3(1, 1, 1)[0]);
					glBindVertexArray(smallSquareVAO);
					glDrawElements(GL_TRIANGLES, smallSquareIndices.size(), GL_UNSIGNED_INT, 0);
					count++;
				}

				if (j == -2 && k == -2) {

					off = glm::vec3(blockOffset.x + ox + ax, 0, blockOffset.z + oz + az);
					glUniform3fv(glGetUniformLocation(programID, "offset"), 1, &off[0]);
					glUniform3fv(glGetUniformLocation(programID, "color"), 1, &glm::vec3(1, 0, 0)[0]);
					glBindVertexArray(outerDegenerateVAO);
					glDrawElements(GL_TRIANGLES, outerDegenerateIndices.size(), GL_UNSIGNED_INT, 0);
					count++;
				}
			}
		}
		scale *= 2;
	}

	//std::cout << "Draw call count for the terrain: " << count << std::endl;
	//std::cout << "Pos x: " << camPoint.x << "z: " << camPoint.z << std::endl;
	glBindVertexArray(0);

}

void TerrainGenerator::drawInGame(GameCamera* camera) {

	int count = 0;
	int scale = 1;
	int level = 6;
	int worldSize = patchSize * 2 * (2 << (level - 1));

	GLCall(glUseProgram(programID));

	glUniformMatrix4fv(glGetUniformLocation(programID, "V"), 1, GL_FALSE, &camera->ViewMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(programID, "P"), 1, GL_FALSE, &camera->ProjectionMatrix[0][0]);
	glUniform1i(glGetUniformLocation(programID, "patchRes"), patchSize);
	glUniform1i(glGetUniformLocation(programID, "mapSize"), elevationMapSize);
	glUniform1i(glGetUniformLocation(programID, "clipMapSize"), worldSize);
	GLCall(glUniform3fv(glGetUniformLocation(programID, "camPos"), 1, &camera->transform->globalPosition[0]));

	glm::vec3 lightDir = glm::normalize(-glm::vec3(1, -1, -1));
	GLCall(glUniform3fv(glGetUniformLocation(programID, "lightDir"), 1, &lightDir[0]));
	//GLCall(glUniform3fv(glGetUniformLocation(programID, "dirLightColor"), 1, &glm::vec3(1, 1, 1)[0]));
	//GLCall(glUniform1f(glGetUniformLocation(programID, "dirLightPower"), 100));

	//GLCall(glUniform3fv(glGetUniformLocation(programID, "lightPosition"), 1, &glm::vec3(pos, 10, pos)[0]));
	GLCall(glUniform3fv(glGetUniformLocation(programID, "lightColor"), 1, &glm::vec3(1, 1, 1)[0]));

	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, elevationMapTexture));
	GLCall(glUniform1i(glGetUniformLocation(programID, "heightmap"), 0));


	GLCall(glActiveTexture(GL_TEXTURE1));
	GLCall(glBindTexture(GL_TEXTURE_2D, grass_a_tex));
	GLCall(glUniform1i(glGetUniformLocation(programID, "grass_a_tex"), 1));

	GLCall(glActiveTexture(GL_TEXTURE2));
	GLCall(glBindTexture(GL_TEXTURE_2D, grass_ao_tex));
	GLCall(glUniform1i(glGetUniformLocation(programID, "grass_ao_tex"), 2));

	GLCall(glActiveTexture(GL_TEXTURE3));
	GLCall(glBindTexture(GL_TEXTURE_2D, grass_n_tex));
	GLCall(glUniform1i(glGetUniformLocation(programID, "grass_n_tex"), 3));

	GLCall(glActiveTexture(GL_TEXTURE4));
	GLCall(glBindTexture(GL_TEXTURE_2D, grass_r_tex));
	GLCall(glUniform1i(glGetUniformLocation(programID, "grass_r_tex"), 4));

	for (int i = 0; i < level; i++) {

		float ox = int((camera->transform->globalPosition.x / 2) * (1 / (float)(1 << i))) * (2 / (float)(patchSize));
		float oz = int((camera->transform->globalPosition.z / 2) * (1 / (float)(1 << i))) * (2 / (float)(patchSize));
		float x = -((int)(ox * 4) % 2) * 4;
		float z = -((int)(oz * 4) % 2) * 4;
		float ax = -((int)(ox * 4) % 2) * (2 / (float)patchSize);
		float az = -((int)(oz * 4) % 2) * (2 / (float)patchSize);

		for (int j = -2; j < 2; j++) {

			for (int k = -2; k < 2; k++) {

				if (i != 0) if (k == -1 || k == 0) if (j == -1 || j == 0) continue;

				glm::vec3 blockOffset(j, 0, k);
				if (j == -1 || j == 1) blockOffset.x -= (float)1 / patchSize;
				if (k == -1 || k == 1) blockOffset.z -= (float)1 / patchSize;

				blockOffset.x += (float)2 / patchSize;
				blockOffset.z += (float)2 / patchSize;

				glm::vec3 off(blockOffset.x + ox, 0, blockOffset.z + oz);
				glUniform1i(glGetUniformLocation(programID, "scale"), scale);
				glUniform3fv(glGetUniformLocation(programID, "offset"), 1, &off[0]);
				glUniform3fv(glGetUniformLocation(programID, "color"), 1, &glm::vec3(0, 0, 1)[0]);
				glBindVertexArray(blockVAO);
				GLCall(glDrawElements(GL_TRIANGLES, blockIndices.size(), GL_UNSIGNED_INT, 0));
				count++;

				if (j == 0) {

					off = glm::vec3(blockOffset.x - (float)2 / patchSize + ox, 0, blockOffset.z + oz);
					glUniform3fv(glGetUniformLocation(programID, "offset"), 1, &off[0]);
					glUniform3fv(glGetUniformLocation(programID, "color"), 1, &glm::vec3(0, 1, 0)[0]);
					glBindVertexArray(ringFixUpHorizontalVAO);
					glDrawElements(GL_TRIANGLES, ringFixUpHorizontalIndices.size(), GL_UNSIGNED_INT, 0);
					count++;

					if (i == 0 && k == 0) {

						off.z -= (float)2 / patchSize;
						glUniform3fv(glGetUniformLocation(programID, "offset"), 1, &off[0]);
						glBindVertexArray(smallSquareVAO);
						glDrawElements(GL_TRIANGLES, smallSquareIndices.size(), GL_UNSIGNED_INT, 0);
						count++;
					}
				}
				else if (j == 1) {

					off = glm::vec3(blockOffset.x + 1 - 1 / (float)patchSize + ox + x, 0, blockOffset.z + oz);
					glUniform3fv(glGetUniformLocation(programID, "offset"), 1, &off[0]);
					glUniform3fv(glGetUniformLocation(programID, "color"), 1, &glm::vec3(0, 1, 1)[0]);
					glBindVertexArray(ringFixUpHorizontalVAO);
					glDrawElements(GL_TRIANGLES, ringFixUpHorizontalIndices.size(), GL_UNSIGNED_INT, 0);
					count++;

					if (k == 0) {

						off.z -= (float)2 / patchSize;
						glUniform3fv(glGetUniformLocation(programID, "offset"), 1, &off[0]);
						glBindVertexArray(smallSquareVAO);
						glDrawElements(GL_TRIANGLES, smallSquareIndices.size(), GL_UNSIGNED_INT, 0);
						count++;
					}
				}

				if (k == 1) {

					off = glm::vec3(blockOffset.x + ox, 0, blockOffset.z + 1 - 1 / (float)patchSize + oz + z);
					glUniform3fv(glGetUniformLocation(programID, "offset"), 1, &off[0]);
					glUniform3fv(glGetUniformLocation(programID, "color"), 1, &glm::vec3(0, 1, 1)[0]);
					glBindVertexArray(ringFixUpVerticalVAO);
					glDrawElements(GL_TRIANGLES, ringFixUpVerticalIndices.size(), GL_UNSIGNED_INT, 0);
					count++;

					if (j == 0) {

						off.x -= (float)2 / patchSize;
						glUniform3fv(glGetUniformLocation(programID, "offset"), 1, &off[0]);
						glBindVertexArray(smallSquareVAO);
						glDrawElements(GL_TRIANGLES, smallSquareIndices.size(), GL_UNSIGNED_INT, 0);
						count++;
					}
				}
				else if (k == 0) {

					off = glm::vec3(blockOffset.x + ox, 0, blockOffset.z - (float)2 / patchSize + oz);
					glUniform3fv(glGetUniformLocation(programID, "offset"), 1, &off[0]);
					glUniform3fv(glGetUniformLocation(programID, "color"), 1, &glm::vec3(1, 0.5, 0)[0]);
					glBindVertexArray(ringFixUpVerticalVAO);
					glDrawElements(GL_TRIANGLES, ringFixUpVerticalIndices.size(), GL_UNSIGNED_INT, 0);
					count++;
				}

				if (j == 1 && k == 1) {

					off = glm::vec3(blockOffset.x + 1 - 1 / (float)patchSize + ox + x, 0, blockOffset.z + 1 - 1 / (float)patchSize + oz + z);
					glUniform3fv(glGetUniformLocation(programID, "offset"), 1, &off[0]);
					glUniform3fv(glGetUniformLocation(programID, "color"), 1, &glm::vec3(1, 1, 1)[0]);
					glBindVertexArray(smallSquareVAO);
					glDrawElements(GL_TRIANGLES, smallSquareIndices.size(), GL_UNSIGNED_INT, 0);
					count++;
				}

				if (j == -2 && k == -2) {

					off = glm::vec3(blockOffset.x + ox + ax, 0, blockOffset.z + oz + az);
					glUniform3fv(glGetUniformLocation(programID, "offset"), 1, &off[0]);
					glUniform3fv(glGetUniformLocation(programID, "color"), 1, &glm::vec3(1, 0, 0)[0]);
					glBindVertexArray(outerDegenerateVAO);
					glDrawElements(GL_TRIANGLES, outerDegenerateIndices.size(), GL_UNSIGNED_INT, 0);
					count++;
				}
			}
		}
		scale *= 2;
	}

	//std::cout << "Draw call count for the terrain: " << count << std::endl;
	//std::cout << "Pos x: " << camPoint.x << "z: " << camPoint.z << std::endl;
	//camPoint.x += 0.002;
	//camPoint.z += 0.01;
	glBindVertexArray(0);
}

void TerrainGenerator::setBrushRadius(float step) {

	if (brushRadius + step > 1.f && brushRadius + step < 100.f) {

		brushRadius += step;
	}
}

void TerrainGenerator::setBrushIntensity(float step) {

	if (brushIntensity + step > 1.f && brushIntensity + step < 100.f) {

		brushIntensity += step;
	}
}

void TerrainGenerator::createBrushGizmo() {

	gizmoShaderProgramID = ShaderNS::loadShaders("source/shader/Line.vert", "source/shader/Line.frag");

	std::vector<glm::vec3> points;

	glm::vec3 center(0,0,0);
	double radius = 1;
	int resolution = BRUSH_RESOLUTION;
	float amount = (2 * PI) / resolution;

	float angle = 0;
	for (int i = 0; i <= resolution; i++) {

		points.push_back(glm::vec3(center.x + radius * cos(angle), 0, center.y + radius * sin(angle)));
		angle += (2 * PI) / resolution;
	}

	glGenVertexArrays(1, &gizmoVAO);
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindVertexArray(gizmoVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), &points[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}