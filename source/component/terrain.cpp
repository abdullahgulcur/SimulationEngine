#include "editor.hpp"
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

	TerrainGenerator::createColorMap();
	TerrainGenerator::createHeightMap();
	TerrainGenerator::createNormalMap();
	TerrainGenerator::generateTerrainClipmapsVertexArrays();
	TerrainGenerator::addNewPalette(grass_a_tex, grass_n_tex);
}

void TerrainGenerator::init(MaterialFile* mat) {

	TerrainGenerator::createColorMap();
	TerrainGenerator::createHeightMap();
	TerrainGenerator::createNormalMap();
	TerrainGenerator::generateTerrainClipmapsVertexArrays();
}

float** TerrainGenerator::getFlatHeightmap() {

	int mipmaps = 0;
	for (int i = elevationMapSize; i > 1; i >>= 1)
		mipmaps++;

	float** heightMipMaps = new float* [mipmaps];

	mipmaps = 0;
	for (int size = elevationMapSize; size > 1; size >>= 1) {

		float* data = new float[size * size];

		for (int i = 0; i < size; i++)
			for (int j = 0; j < size; j++)
				data[i * size + j] = 0;

		heightMipMaps[mipmaps] = data;
		mipmaps++;
	}

	return heightMipMaps;
}

unsigned char* TerrainGenerator::getColorMap(int size) {

	unsigned char* data = new unsigned char[size * size];

	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
			data[i * size + j] = 0;

	return data;
}

char* TerrainGenerator::getNormalMap(int size) {

	char* data = new char[size * size * 3];

	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++) {

			data[(i * size + j) * 3] = 0;
			data[(i * size + j) * 3 + 1] = 127;
			data[(i * size + j) * 3 + 2] = 0;
		}

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

void TerrainGenerator::addNewPalette(unsigned int albedoTexture, unsigned int normalTexture) {

	albedoMaps.push_back(albedoTexture);
	normalMaps.push_back(normalTexture);
	paletteIndices.push_back(0);
}

void TerrainGenerator::removePalette(int index) {

	if (index == texturePaletteIndex)
		texturePaletteIndex = 0;

	paletteIndices.erase(paletteIndices.begin() + index);
	albedoMaps.erase(albedoMaps.begin() + index);
	normalMaps.erase(normalMaps.begin() + index);
}

void TerrainGenerator::selectAllTexturesFromPalette(int index) {

	paletteIndices[index] = 0;
}

void TerrainGenerator::disableAllTexturesFromPalette(int index) {

	paletteIndices[index] = 65535;
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

	float elevation = heightMipMaps[0][heightIndex];

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

			int x_inKernel = x * (float)GFILTER_SIZE / (brushRadius * 2);
			int z_inKernel = z * (float)GFILTER_SIZE / (brushRadius * 2);

			float elevation = GKernel[x_inKernel][z_inKernel] * 50 * brushIntensity;

			int x_denormalized = (int)posInTex.x;
			int z_denormalized = (int)posInTex.z;
			int index = z_denormalized * elevationMapSize + x_denormalized;

			if (index > 0 && index < elevationMapSize * elevationMapSize) {

				int counter = 0;
				for (int size = elevationMapSize; size > 1; size >>= 1) {

					int index = (z_denormalized >> counter) * size + (x_denormalized >> counter);
					heightMipMaps[counter][index] += elevation;
					counter++;
				}
				TerrainGenerator::recalculateNormal(x_denormalized, z_denormalized);
			}
		}
	}

	TerrainGenerator::recreateHeightMap();
	TerrainGenerator::recreateNormalMap();
}

void TerrainGenerator::paint() {

	std::vector<UINT8> indices;

	for (int i = 0; i < 16; i++) {

		unsigned short current = 1 << i;
		int filled = (~paletteIndices[texturePaletteIndex]) & current;
		if (filled) indices.push_back(i);
	}

	for (int i = -brushRadius; i < brushRadius; i++) {

		for (int j = -brushRadius; j < brushRadius; j++) {

			if (i * i + j * j < brushRadius * brushRadius) {

				glm::vec3 posInTex(brushIntersectionPoint->x + i, 0, brushIntersectionPoint->z + j);
				int index = (int)posInTex.z * elevationMapSize + (int)posInTex.x;

				if (index > 0 && index < elevationMapSize * elevationMapSize) {

					for (int k = 0; k < 16; k++) {

						bool active = activeSlopes & (1 << k);
						if (active) {

							glm::vec3 normal(normals[index * 3], normals[index * 3 + 1], normals[index * 3 + 2]);
							normal = glm::normalize(normal);
							float angle = TerrainGenerator::angleBetweenTwoVectors(normal, glm::vec3(0, 1, 0));

							if (angle >= slopeFilters[k].x && angle < slopeFilters[k].y) {

								int randIndex = Utility::getRandomInt(0, indices.size() - 1);
								int randColor = indices[randIndex];
								colors[index] = randColor;
							}
						}
					}

					for (int k = 0; k < 16; k++) {

						bool active = activeHeights & (1 << k);
						if (active) {

							float height = heightMipMaps[0][index];
							if (height >= heightFilters[k].x && height < heightFilters[k].y) {

								int randIndex = Utility::getRandomInt(0, indices.size() - 1);
								int randColor = indices[randIndex];
								colors[index] = randColor;
							}
						}
					}
				}
			}
		}
	}

	TerrainGenerator::recreateColorMap();
}

float TerrainGenerator::angleBetweenTwoVectors(glm::vec3 v0, glm::vec3 v1) {

	return glm::degrees(glm::acos(glm::dot(v0, v1)));
}

void TerrainGenerator::grass() {


}

void TerrainGenerator::tree() {


}

void TerrainGenerator::recalculateNormal(int x, int z) {

	// easy way
	int index0 = (z - 1) * elevationMapSize + x;
	int index1 = z * elevationMapSize + x - 1;
	int index2 = z * elevationMapSize + x + 1;
	int index3 = (z + 1) * elevationMapSize + x;

	/*float h0 = heights[index0];
	float h1 = heights[index1];
	float h2 = heights[index2];
	float h3 = heights[index3];*/

	float h0 = heightMipMaps[0][index0];
	float h1 = heightMipMaps[0][index1];
	float h2 = heightMipMaps[0][index2];
	float h3 = heightMipMaps[0][index3];

	glm::vec3 normal;
	normal.z = h0 - h3;
	normal.x = h1 - h2;
	normal.y = 2;

	normal = glm::normalize(normal);

	normals[(z * elevationMapSize + x) * 3] = (char)(normal.x * 127);
	normals[(z * elevationMapSize + x) * 3 + 1] = (char)(normal.y * 127);
	normals[(z * elevationMapSize + x) * 3 + 2] = (char)(normal.z * 127);
	
	//// sobel
	//float tl = heights[(z - 1) * elevationMapSize + (x - 1)]; // top left
	//float l  = heights[z * elevationMapSize + (x - 1)]; // left
	//float bl = heights[(z + 1) * elevationMapSize + (x - 1)]; // bottom left
	//float t  = heights[(z - 1) * elevationMapSize + x]; // top
	//float b  = heights[(z + 1) * elevationMapSize + x]; // bottom
	//float tr = heights[(z - 1) * elevationMapSize + (x + 1)]; // top right
	//float r  = heights[z * elevationMapSize + (x + 1)]; // right
	//float br = heights[(z + 1) * elevationMapSize + (x + 1)]; // bottom right
 
	//float dX = tr + 2 * r + br - tl - 2 * l - bl;   
	//float dY = bl + 2 * b + br - tl - 2 * t - tr;

	//float normalStrength = 1.f;
	//glm::vec3 N = glm::normalize(glm::vec3(dX, 2.0 / normalStrength, dY));

	//normals[(z * elevationMapSize + x) * 3] = (char)(N.x * 127);
	//normals[(z * elevationMapSize + x) * 3 + 1] = (char)(N.y * 127);
	//normals[(z * elevationMapSize + x) * 3 + 2] = (char)(N.z * 127);
}


void TerrainGenerator::recreateHeightMap() {

	glDeleteTextures(1, &elevationMapTexture);
	glGenTextures(1, &elevationMapTexture);
	glBindTexture(GL_TEXTURE_2D, elevationMapTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, elevationMapSize, elevationMapSize, 0, GL_RED, GL_FLOAT, heights);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, elevationMapSize, elevationMapSize, 0, GL_RED, GL_FLOAT, heightMipMaps[0]);
	glGenerateMipmap(GL_TEXTURE_2D);
}

void TerrainGenerator::recreateColorMap() {

	glDeleteTextures(1, &colorMapTexture);
	glGenTextures(1, &colorMapTexture);
	glBindTexture(GL_TEXTURE_2D, colorMapTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, elevationMapSize, elevationMapSize, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, colors);
	glGenerateMipmap(GL_TEXTURE_2D);
}

void TerrainGenerator::recreateNormalMap() {

	glDeleteTextures(1, &normalMapTexture);
	glGenTextures(1, &normalMapTexture);
	glBindTexture(GL_TEXTURE_2D, normalMapTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, elevationMapSize, elevationMapSize, 0, GL_RGB, GL_BYTE, normals);
	glGenerateMipmap(GL_TEXTURE_2D);
}

void TerrainGenerator::createNormalMap() {

	glGenTextures(1, &normalMapTexture);
	glBindTexture(GL_TEXTURE_2D, normalMapTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	normals = TerrainGenerator::getNormalMap(elevationMapSize);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, elevationMapSize, elevationMapSize, 0, GL_RGB, GL_BYTE, normals);
	glGenerateMipmap(GL_TEXTURE_2D);
}

void TerrainGenerator::createColorMap() {

	glGenTextures(1, &colorMapTexture);
	glBindTexture(GL_TEXTURE_2D, colorMapTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	colors = TerrainGenerator::getColorMap(elevationMapSize);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, elevationMapSize, elevationMapSize, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, colors);
	glGenerateMipmap(GL_TEXTURE_2D);
}

void TerrainGenerator::createHeightMap() {

	glGenTextures(1, &elevationMapTexture);
	glBindTexture(GL_TEXTURE_2D, elevationMapTexture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	//heights = TerrainGenerator::getFlatHeightmap(elevationMapSize);
	heightMipMaps = TerrainGenerator::getFlatHeightmap();

	//glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, elevationMapSize, elevationMapSize, 0, GL_RED, GL_FLOAT, heights);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, elevationMapSize, elevationMapSize, 0, GL_RED, GL_FLOAT, heightMipMaps[0]);
	glGenerateMipmap(GL_TEXTURE_2D);
	//delete[] data;

	grass_a_tex = TextureNS::loadDDS("resource/textures/atlas_a.DDS");
	grass_n_tex = TextureNS::loadDDS("resource/textures/atlas_n.DDS");

	programID = ShaderNS::loadShaders("source/shader/clipmap.vert", "source/shader/clipmap.frag");

}

void TerrainGenerator::getMaxAndMinHeights(glm::vec2& bounds, const int& level, const glm::vec3& start, const glm::vec3& end) {

	bounds.x = 999999;
	bounds.y = -999999;

	int startX = (int)start.x >> level;
	int endX = (int)end.x >> level;
	int startZ = (int)start.z >> level;
	int endZ = (int)end.z >> level;
	int mipMapsize = elevationMapSize >> level;
	for (int i = startZ; i < endZ; i++) {

		for (int j = startX; j < endX; j++) {

			float height = heightMipMaps[level][i * mipMapsize + j];
			if (height < bounds.x) bounds.x = height;
			if (height > bounds.y) bounds.y = height;
		}
	}
}

void TerrainGenerator::setClipmapResolution(int resolution) {

	if (resolution < 0 || resolution > 256)
		return;
	clipmapResolution_temp = resolution;
}

void TerrainGenerator::setClipmapLevel(int level) {

	if (level < 1 || level > 10)
		return;
	clipmapLevel = level;
}

void TerrainGenerator::setBoundariesOfClipmap(const int& level, glm::vec3& start, glm::vec3& end) {

	glm::vec2 bounds;
	TerrainGenerator::getMaxAndMinHeights(bounds, level, start, end);
	start.y = bounds.x;
	end.y = bounds.y;
}

void TerrainGenerator::drawTerrainClipmapAABB(const glm::vec3& start, const glm::vec3& end, const SceneCamera* camera, const Editor* editor) {

	glm::vec3 center = (start + end) * 0.5f;
	glm::mat4 model = glm::translate(glm::mat4(1.0f), center) * glm::scale(glm::mat4(1.0f), start - end);
	editor->gizmo->drawAABB_Box(camera->projectionViewMatrix * model);
}

void TerrainGenerator::drawInScene(SceneCamera* camera, GameCamera* gc, Editor* editor) {

	int count = 0;
	float scale = 1;
	int level = clipmapLevel;
	int clipmapResolution = this->clipmapResolution;
	int worldSize = clipmapResolution * (2 << level);
	unsigned int elevationMapSize = this->elevationMapSize;
	int programID = this->programID;
	float* pvAddr = &camera->projectionViewMatrix[0][0];
	
	// Change this for debugging purposes--------
	glm::vec3 camPos = camera->position;
	//glm::vec3 camPos = gc->getPosition();
	//-------------------------------------------

	float* lightDirAddr = &glm::normalize(-glm::vec3(1, -1, -1))[0];
	float* lightColAddr = &glm::vec3(1, 1, 1)[0];

	int elevationMapTexture = this->elevationMapTexture;
	int colorMapTexture = this->colorMapTexture;
	int normalMapTexture = this->normalMapTexture;
	int grass_a_tex = this->grass_a_tex;
	int grass_n_tex = this->grass_n_tex;

	glUseProgram(programID);
	glUniformMatrix4fv(glGetUniformLocation(programID, "PV"), 1, GL_FALSE, pvAddr);
	glUniform1i(glGetUniformLocation(programID, "patchRes"), level);
	glUniform1i(glGetUniformLocation(programID, "mapSize"), elevationMapSize);
	glUniform1i(glGetUniformLocation(programID, "clipMapSize"), worldSize);
	glUniform3fv(glGetUniformLocation(programID, "camPos"), 1, &camPos[0]);
	glUniform3fv(glGetUniformLocation(programID, "lightDir"), 1, lightDirAddr);
	glUniform3fv(glGetUniformLocation(programID, "lightColor"), 1, lightColAddr);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, elevationMapTexture);
	glUniform1i(glGetUniformLocation(programID, "heightmap"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, colorMapTexture);
	glUniform1i(glGetUniformLocation(programID, "colormap"), 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, normalMapTexture);
	glUniform1i(glGetUniformLocation(programID, "normalmap"), 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, grass_a_tex);
	glUniform1i(glGetUniformLocation(programID, "grass_a_tex"), 3);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, grass_n_tex);
	glUniform1i(glGetUniformLocation(programID, "grass_n_tex"), 4);

	glm::vec2 camDistBoundaries;
	camDistBoundaries.x = worldSize * 0.51f; // 0.51 cok gecici bir cozum...
	camDistBoundaries.y = elevationMapSize - worldSize * 0.51f;
	camPos.x = std::clamp(camPos.x, camDistBoundaries.x, camDistBoundaries.y);
	camPos.z = std::clamp(camPos.z, camDistBoundaries.x, camDistBoundaries.y);

	float triSize = 1.0f;
	float amount = 2.f * triSize;

	for (int i = 0; i < level; i++) {

		float ox = int(camPos.x / (amount * (1 << i))) * (2.f / clipmapResolution);
		float oz = int(camPos.z / (amount * (1 << i))) * (2.f / clipmapResolution);
		float x = -((int)(ox * (clipmapResolution / 2.f)) % 2) * 4;
		float z = -((int)(oz * (clipmapResolution / 2.f)) % 2) * 4;
		float ax = -((int)(ox * (clipmapResolution / 2.f)) % 2) * (2.f / clipmapResolution);
		float az = -((int)(oz * (clipmapResolution / 2.f)) % 2) * (2.f / clipmapResolution);

		for (int j = -2; j < 2; j++) {

			for (int k = -2; k < 2; k++) {

				if (i != 0) if (k == -1 || k == 0) if (j == -1 || j == 0) continue;

				glm::vec2 blockOffset(j, k);
				if (j == -1 || j == 1) blockOffset.x -= 1.f / clipmapResolution;
				if (k == -1 || k == 1) blockOffset.y -= 1.f / clipmapResolution;

				blockOffset.x += 2.f / clipmapResolution;
				blockOffset.y += 2.f / clipmapResolution;

				glm::vec3 off((blockOffset.x + ox) * clipmapResolution, 0, (blockOffset.y + oz) * clipmapResolution);

				glm::vec3 start = off * scale;
				glm::vec3 end = off * scale + scale * glm::vec3(clipmapResolution - 1, 0, clipmapResolution - 1);
				TerrainGenerator::setBoundariesOfClipmap(i, start, end);
				//TerrainGenerator::drawTerrainClipmapAABB(start, end, camera, editor);

				glUseProgram(programID);
				glUniform1f(glGetUniformLocation(programID, "scale"), scale);

				//gc->intersectsAABB(start, end)
				if (camera->intersectsAABB(start, end)) {

					glUniform3fv(glGetUniformLocation(programID, "offsetMultPatchRes"), 1, &off[0]);
					//glUniform3fv(glGetUniformLocation(programID, "color_d"), 1, &glm::vec3(0, 0, 1)[0]);
					glBindVertexArray(blockVAO);
					glDrawElements(GL_TRIANGLES, blockIndices.size(), GL_UNSIGNED_INT, 0);
					count++;
				}
				
				if (j == 0) {

					off = glm::vec3((blockOffset.x - 2.f / clipmapResolution + ox) * clipmapResolution, 0, (blockOffset.y + oz) * clipmapResolution);

					glm::vec3 start = off * scale;
					glm::vec3 end = off * scale + scale * glm::vec3(2, 0, clipmapResolution - 1);
					TerrainGenerator::setBoundariesOfClipmap(0, start, end);
					//TerrainGenerator::drawTerrainClipmapAABB(start, end, camera, editor);

					if (camera->intersectsAABB(start, end)) {

						glUseProgram(programID);
						glUniform3fv(glGetUniformLocation(programID, "offsetMultPatchRes"), 1, &off[0]);
						//glUniform3fv(glGetUniformLocation(programID, "color_d"), 1, &glm::vec3(0, 1, 0)[0]);
						glBindVertexArray(ringFixUpHorizontalVAO);
						glDrawElements(GL_TRIANGLES, ringFixUpHorizontalIndices.size(), GL_UNSIGNED_INT, 0);
						count++;
					}

					

					if (i == 0 && k == 0) {

						off.z -= 2;

						glm::vec3 start = off * scale;
						glm::vec3 end = off * scale + scale * glm::vec3(2, 0, 2);
						TerrainGenerator::setBoundariesOfClipmap(0, start, end);
						//TerrainGenerator::drawTerrainClipmapAABB(start, end, i, camera, editor);

						if (camera->intersectsAABB(start, end)) {

							glUseProgram(programID);
							glUniform3fv(glGetUniformLocation(programID, "offsetMultPatchRes"), 1, &off[0]);
							glBindVertexArray(smallSquareVAO);
							glDrawElements(GL_TRIANGLES, smallSquareIndices.size(), GL_UNSIGNED_INT, 0);
							count++;
						}

						
					}
				}
				else if (j == 1) {

					off = glm::vec3((blockOffset.x + 1 - 1.f / clipmapResolution + ox + x) * clipmapResolution, 0, (blockOffset.y + oz) * clipmapResolution);

					glm::vec3 start = off * scale;
					glm::vec3 end = off * scale + scale * glm::vec3(2, 0, clipmapResolution - 1);
					TerrainGenerator::setBoundariesOfClipmap(0, start, end);
					//TerrainGenerator::drawTerrainClipmapAABB(start, end, camera, editor);

					if (camera->intersectsAABB(start, end)) {

						glUseProgram(programID);
						glUniform3fv(glGetUniformLocation(programID, "offsetMultPatchRes"), 1, &off[0]);
						//glUniform3fv(glGetUniformLocation(programID, "color_d"), 1, &glm::vec3(0, 1, 1)[0]);
						glBindVertexArray(ringFixUpHorizontalVAO);
						glDrawElements(GL_TRIANGLES, ringFixUpHorizontalIndices.size(), GL_UNSIGNED_INT, 0);
						count++;
					}

					if (k == 0) {

						off.z -= 2;

						glm::vec3 start = off * scale;
						glm::vec3 end = off * scale + scale * glm::vec3(2, 0, 2);
						TerrainGenerator::setBoundariesOfClipmap(0, start, end);
						//TerrainGenerator::drawTerrainClipmapAABB(start, end, i, camera, editor);

						if (camera->intersectsAABB(start, end)) {

							glUseProgram(programID);
							glUniform3fv(glGetUniformLocation(programID, "offsetMultPatchRes"), 1, &off[0]);
							glBindVertexArray(smallSquareVAO);
							glDrawElements(GL_TRIANGLES, smallSquareIndices.size(), GL_UNSIGNED_INT, 0);
							count++;
						}
						
					}
				}

				if (k == 1) {

					off = glm::vec3((blockOffset.x + ox) * clipmapResolution, 0, (blockOffset.y + 1 - 1.f / clipmapResolution + oz + z) * clipmapResolution);

					glm::vec3 start = off * scale;
					glm::vec3 end = off * scale + scale * glm::vec3(clipmapResolution - 1, 0, 2);
					TerrainGenerator::setBoundariesOfClipmap(0, start, end);
					//TerrainGenerator::drawTerrainClipmapAABB(start, end, camera, editor);

					if (camera->intersectsAABB(start, end)) {

						glUseProgram(programID);
						glUniform3fv(glGetUniformLocation(programID, "offsetMultPatchRes"), 1, &off[0]);
						//glUniform3fv(glGetUniformLocation(programID, "color_d"), 1, &glm::vec3(0, 1, 1)[0]);
						glBindVertexArray(ringFixUpVerticalVAO);
						glDrawElements(GL_TRIANGLES, ringFixUpVerticalIndices.size(), GL_UNSIGNED_INT, 0);
						count++;
					}
					

					if (j == 0) {

						off.x -= 2;

						glm::vec3 start = off * scale;
						glm::vec3 end = off * scale + scale * glm::vec3(2, 0, 2);
						TerrainGenerator::setBoundariesOfClipmap(0, start, end);
						//TerrainGenerator::drawTerrainClipmapAABB(start, end, i, camera, editor);

						if (camera->intersectsAABB(start, end)) {

							glUseProgram(programID);
							glUniform3fv(glGetUniformLocation(programID, "offsetMultPatchRes"), 1, &off[0]);
							glBindVertexArray(smallSquareVAO);
							glDrawElements(GL_TRIANGLES, smallSquareIndices.size(), GL_UNSIGNED_INT, 0);
							count++;
						}
					}
				}
				else if (k == 0) {

					off = glm::vec3((blockOffset.x + ox) * clipmapResolution, 0, (blockOffset.y - 2.f / clipmapResolution + oz) * clipmapResolution);

					glm::vec3 start = off * scale;
					glm::vec3 end = off * scale + scale * glm::vec3(clipmapResolution - 1, 0, 2);
					TerrainGenerator::setBoundariesOfClipmap(0, start, end);
					//TerrainGenerator::drawTerrainClipmapAABB(start, end, camera, editor);

					if (camera->intersectsAABB(start, end)) {

						glUseProgram(programID);
						glUniform3fv(glGetUniformLocation(programID, "offsetMultPatchRes"), 1, &off[0]);
						//glUniform3fv(glGetUniformLocation(programID, "color_d"), 1, &glm::vec3(1, 0.5, 0)[0]);
						glBindVertexArray(ringFixUpVerticalVAO);
						glDrawElements(GL_TRIANGLES, ringFixUpVerticalIndices.size(), GL_UNSIGNED_INT, 0);
						count++;
					}
				}

				if (j == 1 && k == 1) {

					off = glm::vec3((blockOffset.x + 1 - 1.f / clipmapResolution + ox + x) * clipmapResolution, 0, (blockOffset.y + 1 - 1.f / clipmapResolution + oz + z) * clipmapResolution);

					glm::vec3 start = off * scale;
					glm::vec3 end = off * scale + scale * glm::vec3(2, 0, 2);
					TerrainGenerator::setBoundariesOfClipmap(0, start, end);
					//TerrainGenerator::drawTerrainClipmapAABB(start, end, i, camera, editor);

					if (camera->intersectsAABB(start, end)) {

						glUseProgram(programID);
						glUniform3fv(glGetUniformLocation(programID, "offsetMultPatchRes"), 1, &off[0]);
						//glUniform3fv(glGetUniformLocation(programID, "color_d"), 1, &glm::vec3(1, 1, 1)[0]);
						glBindVertexArray(smallSquareVAO);
						glDrawElements(GL_TRIANGLES, smallSquareIndices.size(), GL_UNSIGNED_INT, 0);
						count++;
					}
					
				}

				if (j == -2 && k == -2) {

					off = glm::vec3((blockOffset.x + ox + ax) * clipmapResolution, 0, (blockOffset.y + oz + az) * clipmapResolution);
					glUniform3fv(glGetUniformLocation(programID, "offsetMultPatchRes"), 1, &off[0]);
					//glUniform3fv(glGetUniformLocation(programID, "color_d"), 1, &glm::vec3(1, 0, 0)[0]);
					glBindVertexArray(outerDegenerateVAO);
					glDrawElements(GL_TRIANGLES, outerDegenerateIndices.size(), GL_UNSIGNED_INT, 0);
					count++;
				}
			}
		}
		scale *= 2;
	}

	//std::cout << "Draw call count for the terrain: " << count << std::endl;
	glBindVertexArray(0);

}

void TerrainGenerator::drawInGame(GameCamera* camera) {

	int count = 0;
	float scale = 1;
	int level = clipmapLevel;
	int clipmapResolution = this->clipmapResolution;
	int worldSize = clipmapResolution * (2 << level);
	unsigned int elevationMapSize = this->elevationMapSize;
	int programID = this->programID;
	float* pvAddr = &camera->projectionViewMatrix[0][0];
	glm::vec3 camPos = camera->getPosition();
	float* lightDirAddr = &glm::normalize(-glm::vec3(1, -1, -1))[0];
	float* lightColAddr = &glm::vec3(1, 1, 1)[0];

	int elevationMapTexture = this->elevationMapTexture;
	int colorMapTexture = this->colorMapTexture;
	int normalMapTexture = this->normalMapTexture;
	int grass_a_tex = this->grass_a_tex;
	int grass_n_tex = this->grass_n_tex;

	glUseProgram(programID);
	glUniformMatrix4fv(glGetUniformLocation(programID, "PV"), 1, GL_FALSE, pvAddr);
	glUniform1i(glGetUniformLocation(programID, "patchRes"), level);
	glUniform1i(glGetUniformLocation(programID, "mapSize"), elevationMapSize);
	glUniform1i(glGetUniformLocation(programID, "clipMapSize"), worldSize);
	glUniform3fv(glGetUniformLocation(programID, "camPos"), 1, &camPos[0]);
	glUniform3fv(glGetUniformLocation(programID, "lightDir"), 1, lightDirAddr);
	glUniform3fv(glGetUniformLocation(programID, "lightColor"), 1, lightColAddr);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, elevationMapTexture);
	glUniform1i(glGetUniformLocation(programID, "heightmap"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, colorMapTexture);
	glUniform1i(glGetUniformLocation(programID, "colormap"), 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, normalMapTexture);
	glUniform1i(glGetUniformLocation(programID, "normalmap"), 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, grass_a_tex);
	glUniform1i(glGetUniformLocation(programID, "grass_a_tex"), 3);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, grass_n_tex);
	glUniform1i(glGetUniformLocation(programID, "grass_n_tex"), 4);

	glm::vec2 camDistBoundaries;
	camDistBoundaries.x = worldSize * 0.51f; // 0.51 cok gecici bir cozum...
	camDistBoundaries.y = elevationMapSize - worldSize * 0.51f;
	camPos.x = std::clamp(camPos.x, camDistBoundaries.x, camDistBoundaries.y);
	camPos.z = std::clamp(camPos.z, camDistBoundaries.x, camDistBoundaries.y);

	for (int i = 0; i < level; i++) {

		float ox = int(camPos.x / (2 << i)) * (2.f / clipmapResolution);
		float oz = int(camPos.z / (2 << i)) * (2.f / clipmapResolution);
		float x = -((int)(ox * (clipmapResolution / 2.f)) % 2) * 4;
		float z = -((int)(oz * (clipmapResolution / 2.f)) % 2) * 4;
		float ax = -((int)(ox * (clipmapResolution / 2.f)) % 2) * (2.f / clipmapResolution);
		float az = -((int)(oz * (clipmapResolution / 2.f)) % 2) * (2.f / clipmapResolution);

		for (int j = -2; j < 2; j++) {

			for (int k = -2; k < 2; k++) {

				if (i != 0) if (k == -1 || k == 0) if (j == -1 || j == 0) continue;

				glm::vec2 blockOffset(j, k);
				if (j == -1 || j == 1) blockOffset.x -= 1.f / clipmapResolution;
				if (k == -1 || k == 1) blockOffset.y -= 1.f / clipmapResolution;

				blockOffset.x += 2.f / clipmapResolution;
				blockOffset.y += 2.f / clipmapResolution;

				glm::vec3 off((blockOffset.x + ox) * clipmapResolution, 0, (blockOffset.y + oz) * clipmapResolution);

				glm::vec3 start = off * scale;
				glm::vec3 end = off * scale + scale * glm::vec3(clipmapResolution - 1, 0, clipmapResolution - 1);
				TerrainGenerator::setBoundariesOfClipmap(i, start, end);

				glUniform1f(glGetUniformLocation(programID, "scale"), scale);

				if (camera->intersectsAABB(start, end)) {

					glUniform3fv(glGetUniformLocation(programID, "offsetMultPatchRes"), 1, &off[0]);
					glBindVertexArray(blockVAO);
					glDrawElements(GL_TRIANGLES, blockIndices.size(), GL_UNSIGNED_INT, 0);
				}

				if (j == 0) {

					off = glm::vec3((blockOffset.x - 2.f / clipmapResolution + ox) * clipmapResolution, 0, (blockOffset.y + oz) * clipmapResolution);
					glm::vec3 start = off * scale;
					glm::vec3 end = off * scale + scale * glm::vec3(2, 0, clipmapResolution - 1);
					TerrainGenerator::setBoundariesOfClipmap(0, start, end);

					if (camera->intersectsAABB(start, end)) {

						glUniform3fv(glGetUniformLocation(programID, "offsetMultPatchRes"), 1, &off[0]);
						glBindVertexArray(ringFixUpHorizontalVAO);
						glDrawElements(GL_TRIANGLES, ringFixUpHorizontalIndices.size(), GL_UNSIGNED_INT, 0);
					}

					if (i == 0 && k == 0) {

						off.z -= 2;

						glm::vec3 start = off * scale;
						glm::vec3 end = off * scale + scale * glm::vec3(2, 0, 2);
						TerrainGenerator::setBoundariesOfClipmap(0, start, end);

						if (camera->intersectsAABB(start, end)) {

							glUniform3fv(glGetUniformLocation(programID, "offsetMultPatchRes"), 1, &off[0]);
							glBindVertexArray(smallSquareVAO);
							glDrawElements(GL_TRIANGLES, smallSquareIndices.size(), GL_UNSIGNED_INT, 0);
						}
					}
				}
				else if (j == 1) {

					off = glm::vec3((blockOffset.x + 1 - 1.f / clipmapResolution + ox + x) * clipmapResolution, 0, (blockOffset.y + oz) * clipmapResolution);
					glm::vec3 start = off * scale;
					glm::vec3 end = off * scale + scale * glm::vec3(2, 0, clipmapResolution - 1);
					TerrainGenerator::setBoundariesOfClipmap(0, start, end);

					if (camera->intersectsAABB(start, end)) {

						glUniform3fv(glGetUniformLocation(programID, "offsetMultPatchRes"), 1, &off[0]);
						glBindVertexArray(ringFixUpHorizontalVAO);
						glDrawElements(GL_TRIANGLES, ringFixUpHorizontalIndices.size(), GL_UNSIGNED_INT, 0);
					}

					if (k == 0) {

						off.z -= 2;
						glm::vec3 start = off * scale;
						glm::vec3 end = off * scale + scale * glm::vec3(2, 0, 2);
						TerrainGenerator::setBoundariesOfClipmap(0, start, end);

						if (camera->intersectsAABB(start, end)) {

							glUniform3fv(glGetUniformLocation(programID, "offsetMultPatchRes"), 1, &off[0]);
							glBindVertexArray(smallSquareVAO);
							glDrawElements(GL_TRIANGLES, smallSquareIndices.size(), GL_UNSIGNED_INT, 0);
						}
					}
				}

				if (k == 1) {

					off = glm::vec3((blockOffset.x + ox) * clipmapResolution, 0, (blockOffset.y + 1 - 1.f / clipmapResolution + oz + z) * clipmapResolution);
					glm::vec3 start = off * scale;
					glm::vec3 end = off * scale + scale * glm::vec3(clipmapResolution - 1, 0, 2);
					TerrainGenerator::setBoundariesOfClipmap(0, start, end);

					if (camera->intersectsAABB(start, end)) {

						glUniform3fv(glGetUniformLocation(programID, "offsetMultPatchRes"), 1, &off[0]);
						glBindVertexArray(ringFixUpVerticalVAO);
						glDrawElements(GL_TRIANGLES, ringFixUpVerticalIndices.size(), GL_UNSIGNED_INT, 0);
					}


					if (j == 0) {

						off.x -= 2;
						glm::vec3 start = off * scale;
						glm::vec3 end = off * scale + scale * glm::vec3(2, 0, 2);
						TerrainGenerator::setBoundariesOfClipmap(0, start, end);

						if (camera->intersectsAABB(start, end)) {

							glUniform3fv(glGetUniformLocation(programID, "offsetMultPatchRes"), 1, &off[0]);
							glBindVertexArray(smallSquareVAO);
							glDrawElements(GL_TRIANGLES, smallSquareIndices.size(), GL_UNSIGNED_INT, 0);
						}
					}
				}
				else if (k == 0) {

					off = glm::vec3((blockOffset.x + ox) * clipmapResolution, 0, (blockOffset.y - 2.f / clipmapResolution + oz) * clipmapResolution);
					glm::vec3 start = off * scale;
					glm::vec3 end = off * scale + scale * glm::vec3(clipmapResolution - 1, 0, 2);
					TerrainGenerator::setBoundariesOfClipmap(0, start, end);

					if (camera->intersectsAABB(start, end)) {

						glUniform3fv(glGetUniformLocation(programID, "offsetMultPatchRes"), 1, &off[0]);
						glBindVertexArray(ringFixUpVerticalVAO);
						glDrawElements(GL_TRIANGLES, ringFixUpVerticalIndices.size(), GL_UNSIGNED_INT, 0);
					}
				}

				if (j == 1 && k == 1) {

					off = glm::vec3((blockOffset.x + 1 - 1.f / clipmapResolution + ox + x) * clipmapResolution, 0, (blockOffset.y + 1 - 1.f / clipmapResolution + oz + z) * clipmapResolution);
					glm::vec3 start = off * scale;
					glm::vec3 end = off * scale + scale * glm::vec3(2, 0, 2);
					TerrainGenerator::setBoundariesOfClipmap(0, start, end);

					if (camera->intersectsAABB(start, end)) {

						glUniform3fv(glGetUniformLocation(programID, "offsetMultPatchRes"), 1, &off[0]);
						glBindVertexArray(smallSquareVAO);
						glDrawElements(GL_TRIANGLES, smallSquareIndices.size(), GL_UNSIGNED_INT, 0);
					}

				}

				if (j == -2 && k == -2) {

					off = glm::vec3((blockOffset.x + ox + ax) * clipmapResolution, 0, (blockOffset.y + oz + az) * clipmapResolution);
					glUniform3fv(glGetUniformLocation(programID, "offsetMultPatchRes"), 1, &off[0]);
					glBindVertexArray(outerDegenerateVAO);
					glDrawElements(GL_TRIANGLES, outerDegenerateIndices.size(), GL_UNSIGNED_INT, 0);
				}
			}
		}
		scale *= 2;
	}
	glBindVertexArray(0);

}

void TerrainGenerator::setBrushRadius(float step) {

	if (brushRadius + step > 1.f && brushRadius + step < 100.f)
		brushRadius += step;
}

void TerrainGenerator::setBrushIntensity(float step) {

	if (brushIntensity + step > 1.f && brushIntensity + step < 100.f)
		brushIntensity += step;
}

void TerrainGenerator::setSlopeFilters(int size) {

	if (size > 16 || size < 0)
		return;

	if (size > slopeFilters.size())
		slopeFilters.push_back(glm::vec2(0, 0));
	else if (size < slopeFilters.size()) {
		slopeFilters.pop_back();
		activeSlopes &= ~(1 << size);
	}
}

void TerrainGenerator::setHeightFilters(int size) {

	if (size > 16 || size < 0)
		return;

	if (size > heightFilters.size())
		heightFilters.push_back(glm::vec2(0, 0));
	else if (size < heightFilters.size()) {
		heightFilters.pop_back();
		activeHeights &= ~(1 << size);
	}
}

void TerrainGenerator::setPerlinOctaves(int size) {

	if (size > 8 || size < 0)
		return;

	perlinOctaves = (unsigned char)size;
}

void TerrainGenerator::applyPerlinNoise() {

	const siv::PerlinNoise perlin{ perlinSeed };

	for (int z = 0; z < elevationMapSize; z++) {

		for (int x = 0; x < elevationMapSize; x++) {

			float height = perlin.octave2D((double)x * perlinScale, (double)z * perlinScale, perlinOctaves, perlinPersistence) * heightScale;
			int counter = 0;
			for (int size = elevationMapSize; size > 1; size >>= 1) {

				int index = (z >> counter) * size + (x >> counter);
				heightMipMaps[counter][index] = height;
				counter++;
			}
		}
	}

	for (int z = 1; z < elevationMapSize - 1; z++)
		for (int x = 1; x < elevationMapSize - 1; x++)
			TerrainGenerator::recalculateNormal(x, z);

	TerrainGenerator::recreateHeightMap();
	TerrainGenerator::recreateNormalMap();
}

void TerrainGenerator::applyFiltersInWholeMap() {

	std::vector<UINT8> indices;

	for (int i = 0; i < 16; i++) {

		unsigned short current = 1 << i;
		int filled = (~paletteIndices[texturePaletteIndex]) & current;
		if (filled) indices.push_back(i);
	}

	for (int i = 0; i < elevationMapSize; i++) {

		for (int j = 0; j < elevationMapSize; j++) {

			int index = i * elevationMapSize + j;

			for (int k = 0; k < 16; k++) {

				bool active = activeSlopes & (1 << k);
				if (active) {

					glm::vec3 normal(normals[index * 3], normals[index * 3 + 1], normals[index * 3 + 2]);
					normal = glm::normalize(normal);
					float angle = TerrainGenerator::angleBetweenTwoVectors(normal, glm::vec3(0, 1, 0));

					if (angle >= slopeFilters[k].x && angle < slopeFilters[k].y) {

						int randIndex = Utility::getRandomInt(0, indices.size() - 1);
						int randColor = indices[randIndex];
						colors[index] = randColor;
					}
				}
			}

			for (int k = 0; k < 16; k++) {

				bool active = activeHeights & (1 << k);
				if (active) {

					float height = heightMipMaps[0][index];
					if (height >= heightFilters[k].x && height < heightFilters[k].y) {

						int randIndex = Utility::getRandomInt(0, indices.size() - 1);
						int randColor = indices[randIndex];
						colors[index] = randColor;
					}
				}
			}
		}
	}

	TerrainGenerator::recreateColorMap();
}

void TerrainGenerator::generateTerrainClipmapsVertexArrays() {

	std::vector<glm::vec3> verts;
	std::vector<glm::vec3> ringFixUpVerts;
	std::vector<glm::vec3> ringFixUpVerts1;
	std::vector<glm::vec3> smallSquareVerts;
	std::vector<glm::vec3> outerDegenerateVerts;

	for (int i = 0; i < clipmapResolution; i++)
		for (int j = 0; j < clipmapResolution; j++)
			verts.push_back(glm::vec3(j, 0, i));

	for (int i = 0; i < clipmapResolution - 1; i++) {

		for (int j = 0; j < clipmapResolution - 1; j++) {

			blockIndices.push_back(j + i * (clipmapResolution));
			blockIndices.push_back(j + (i + 1) * (clipmapResolution));
			blockIndices.push_back(j + i * (clipmapResolution)+1);

			blockIndices.push_back(j + i * (clipmapResolution)+1);
			blockIndices.push_back(j + (i + 1) * (clipmapResolution));
			blockIndices.push_back(j + (i + 1) * (clipmapResolution)+1);
		}
	}

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			smallSquareVerts.push_back(glm::vec3(j, 0, i));

	for (int i = 0; i < 3 - 1; i++) {

		for (int j = 0; j < 3 - 1; j++) {

			smallSquareIndices.push_back(j + i * (3));
			smallSquareIndices.push_back(j + (i + 1) * (3));
			smallSquareIndices.push_back(j + i * (3) + 1);

			smallSquareIndices.push_back(j + i * (3) + 1);
			smallSquareIndices.push_back(j + (i + 1) * (3));
			smallSquareIndices.push_back(j + (i + 1) * (3) + 1);
		}
	}

	for (int i = 0; i < clipmapResolution; i++)
		for (int j = 0; j < 3; j++)
			ringFixUpVerts.push_back(glm::vec3(j, 0, i));

	for (int i = 0; i < clipmapResolution - 1; i++) {

		for (int j = 0; j < 3 - 1; j++) {

			ringFixUpHorizontalIndices.push_back(j + i * (3));
			ringFixUpHorizontalIndices.push_back(j + (i + 1) * (3));
			ringFixUpHorizontalIndices.push_back(j + i * (3) + 1);

			ringFixUpHorizontalIndices.push_back(j + i * (3) + 1);
			ringFixUpHorizontalIndices.push_back(j + (i + 1) * (3));
			ringFixUpHorizontalIndices.push_back(j + (i + 1) * (3) + 1);
		}
	}

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < clipmapResolution; j++)
			ringFixUpVerts1.push_back(glm::vec3(j, 0, i));

	for (int i = 0; i < 3 - 1; i++) {

		for (int j = 0; j < clipmapResolution - 1; j++) {

			ringFixUpVerticalIndices.push_back(j + i * (clipmapResolution));
			ringFixUpVerticalIndices.push_back(j + (i + 1) * (clipmapResolution));
			ringFixUpVerticalIndices.push_back(j + i * (clipmapResolution)+1);

			ringFixUpVerticalIndices.push_back(j + i * (clipmapResolution)+1);
			ringFixUpVerticalIndices.push_back(j + (i + 1) * (clipmapResolution));
			ringFixUpVerticalIndices.push_back(j + (i + 1) * (clipmapResolution)+1);
		}
	}

	// Outer degenerate triangles (y lere hic gerek yok !!!)
	for (int i = 0; i < clipmapResolution * 4; i += 2) {

		outerDegenerateVerts.push_back(glm::vec3(0, 0, i));
		outerDegenerateVerts.push_back(glm::vec3(0, 0, i + 1));
		outerDegenerateVerts.push_back(glm::vec3(0, 0, i + 2));
	}

	for (int i = 0; i < clipmapResolution * 4; i += 2) {

		outerDegenerateVerts.push_back(glm::vec3(i, 0, clipmapResolution * 4));
		outerDegenerateVerts.push_back(glm::vec3(i + 1, 0, clipmapResolution * 4));
		outerDegenerateVerts.push_back(glm::vec3(i + 2, 0, clipmapResolution * 4));
	}

	for (int i = clipmapResolution * 4; i > 0; i -= 2) {

		outerDegenerateVerts.push_back(glm::vec3(clipmapResolution * 4, 0, i));
		outerDegenerateVerts.push_back(glm::vec3(clipmapResolution * 4, 0, i - 1));
		outerDegenerateVerts.push_back(glm::vec3(clipmapResolution * 4, 0, i - 2));
	}

	for (int i = clipmapResolution * 4; i > 0; i -= 2) {

		outerDegenerateVerts.push_back(glm::vec3(i, 0, 0));
		outerDegenerateVerts.push_back(glm::vec3(i - 1, 0, 0));
		outerDegenerateVerts.push_back(glm::vec3(i - 2, 0, 0));
	}

	for (int i = 0; i < clipmapResolution * 3 * 2 * 4; i++)
		outerDegenerateIndices.push_back(i);

	for (int i = clipmapResolution * 3 * 2 * 4 - 1; i >= 0; i--)
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

void TerrainGenerator::applyClipmapProperties() {

	clipmapResolution = clipmapResolution_temp;

	glDeleteVertexArrays(1, &blockVAO);
	glDeleteVertexArrays(1, &ringFixUpHorizontalVAO);
	glDeleteVertexArrays(1, &ringFixUpVerticalVAO);
	glDeleteVertexArrays(1, &smallSquareVAO);
	glDeleteVertexArrays(1, &outerDegenerateVAO);

	blockIndices.clear();
	ringFixUpHorizontalIndices.clear();
	ringFixUpVerticalIndices.clear();
	smallSquareIndices.clear();
	outerDegenerateIndices.clear();

	TerrainGenerator::generateTerrainClipmapsVertexArrays();
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