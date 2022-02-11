#include "terrain.hpp"
#include "camera.hpp"

TerrainGenerator::TerrainGenerator() {

	//TerrainGenerator::initializeBrushIndices(4);
}

void TerrainGenerator::init() {

	GLint maxTessLevel;
	glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &maxTessLevel);
	std::cout << "Max available tess level: " << maxTessLevel << std::endl;
	TerrainGenerator::createHeightMap();
	//std::vector<TerrainChunk> chunks;
	//std::vector<std::vector<unsigned short>> indices;
	//TerrainGenerator::createHeightMap();
	//TerrainGenerator::createTerrainMesh(chunks, indices);
	//TerrainGenerator::recalculateNormals(chunks, indices);

	//// Debug Purpose Only
	//TerrainGenerator::initDebugNormalLines(chunks, indices);

	//TerrainGenerator::createHeightField(chunks, indices);
}

void TerrainGenerator::init(MaterialFile* mat) {

	GLint maxTessLevel;
	glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &maxTessLevel);
	std::cout << "Max available tess level: " << maxTessLevel << std::endl;
	TerrainGenerator::createHeightMap();
	//this->mat = mat;
	//std::vector<TerrainChunk> chunks;
	//std::vector<std::vector<unsigned short>> indices;
	//TerrainGenerator::createHeightMap();
	//TerrainGenerator::createTerrainMesh(chunks, indices);
	//TerrainGenerator::recalculateNormals(chunks, indices);

	//// Debug Purpose Only
	//TerrainGenerator::initDebugNormalLines(chunks, indices);

	//TerrainGenerator::createHeightField(chunks, indices);
}

void TerrainGenerator::recreateHeightField() {

	for (auto& VAOList : VAOs)
		for (auto& VAO : VAOList)
			glDeleteVertexArrays(1, &VAO);
	VAOs.clear();

	std::vector<TerrainChunk> chunks;
	std::vector<std::vector<unsigned short>> indices;
	TerrainGenerator::createTerrainMesh(chunks, indices);
	TerrainGenerator::recalculateNormals(chunks, indices);

	// Debug Purpose Only
	TerrainGenerator::initDebugNormalLines(chunks, indices);

	TerrainGenerator::createHeightField(chunks, indices);
}

// reference: https://learnopengl.com/Guest-Articles/2021/Tessellation/Tessellation
void TerrainGenerator::createHeightMap() {

	programID = ShaderNS::loadShaders("source/shader/gpuheight.vert", "source/shader/gpuheight.frag",
		"source/shader/gpuheight.tesc", "source/shader/gpuheight.tese");

	// load and create a texture
	// -------------------------
	
	glGenTextures(1, &elevationMapTexture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, elevationMapTexture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	unsigned int width;
	unsigned int height;
	unsigned char* data = TextureNS::loadPNG("resource/heightmaps/rockyriver.png", width, height);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glUniform1i(glGetUniformLocation(programID, "heightMap"), 0);
	delete[] data;

	int w = width;
	int h = height;
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	std::vector<float> vertices;

	for (unsigned i = 0; i <= rez - 1; i++)
	{
		for (unsigned j = 0; j <= rez - 1; j++)
		{
			vertices.push_back(-w / 2.0f + w * i / (float)rez); // v.x
			vertices.push_back(0.0f); // v.y
			vertices.push_back(-h / 2.0f + h * j / (float)rez); // v.z
			vertices.push_back(i / (float)rez); // u
			vertices.push_back(j / (float)rez); // v

			vertices.push_back(-w / 2.0f + w * (i + 1) / (float)rez); // v.x
			vertices.push_back(0.0f); // v.y
			vertices.push_back(-h / 2.0f + h * j / (float)rez); // v.z
			vertices.push_back((i + 1) / (float)rez); // u
			vertices.push_back(j / (float)rez); // v

			vertices.push_back(-w / 2.0f + w * i / (float)rez); // v.x
			vertices.push_back(0.0f); // v.y
			vertices.push_back(-h / 2.0f + h * (j + 1) / (float)rez); // v.z
			vertices.push_back(i / (float)rez); // u
			vertices.push_back((j + 1) / (float)rez); // v

			vertices.push_back(-w / 2.0f + w * (i + 1) / (float)rez); // v.x
			vertices.push_back(0.0f); // v.y
			vertices.push_back(-h / 2.0f + h * (j + 1) / (float)rez); // v.z
			vertices.push_back((i + 1) / (float)rez); // u
			vertices.push_back((j + 1) / (float)rez); // v
		}
	}
	std::cout << "Loaded " << rez * rez << " patches of 4 control points each" << std::endl;
	std::cout << "Processing " << rez * rez * 4 << " vertices in vertex shader" << std::endl;

	// first, configure the cube's VAO (and terrainVBO)
	unsigned int terrainVBO;
	glGenVertexArrays(1, &terrainVAO);
	glBindVertexArray(terrainVAO);

	glGenBuffers(1, &terrainVBO);
	glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texCoord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);

	glPatchParameteri(GL_PATCH_VERTICES, NUM_PATCH_PTS);

	/*unsigned width;
	unsigned height;
	std::vector<unsigned char> elevations;
	unsigned error = lodepng::decode(elevations, width, height, "resource/heightmaps/hh.png", LodePNGColorType::LCT_RGBA, 8);

	for (int i = 0; i < 1024; i++) {

		for (int j = 0; j < 1024; j++) {

			float elevation = (elevations[(i * width + j) * 4] * 16 + elevations[(i * width + j) * 4 + 1]) / 20.f;
			heights.push_back(elevation);
		}
	}

	TerrainGenerator::scaleElevationMap(2);

	TerrainGenerator::filterElevationMap(15, 1);*/
}

//void TerrainGenerator::createHeightMap() {
//
//	const siv::PerlinNoise perlin(Utility::getRandomInt(0, 99999));
//	//const siv::PerlinNoise perlin(999);
//
//	for (int i = 0; i < elevationMapSize; i++) {
//
//		for (int j = 0; j < elevationMapSize; j++) {
//
//			float height = perlin.octave2D_01(i * 0.01f, j * 0.01f, 3) * 100.f;
//			height = height < 40.f ? height = 40.f - ((40.f - height) * 0.8f) : height;
//			height = height < 35.f ? height = 35.f - ((35.f - height) * 0.8f) : height;
//			height = height < 30.f ? height = 30.f - ((30.f - height) * 0.8f) : height;
//
//			if (j != 0.f) {
//				float temp = heights.back() - height;
//				maxHeight = temp > maxHeight ? temp : maxHeight;
//			}
//
//			heights.push_back(height);
//		}
//	}
//
//	TerrainGenerator::erode();
//	TerrainGenerator::scaleElevationMap(2);
//
//	TerrainGenerator::filterElevationMap(15, 1);
//}

void TerrainGenerator::filterElevationMap(int size, int iterations) {

	for (int it = 0; it < iterations; it++) {

		for (int z = size / 2; z < elevationMapSize - size / 2; z++) {

			for (int x = size / 2; x < elevationMapSize - size / 2; x++) {

				double sum = 0;
				for (int i = -size / 2; i < size / 2; i++) {

					for (int j = -size / 2; j < size / 2; j++) {

						sum += heights[(z + i) * elevationMapSize + (x + j)] / (size * size);
					}
				}
				heights[z * elevationMapSize + x] = sum;
			}
		}
	}
}

// Scales elevation map to the bigger sizes.
void TerrainGenerator::scaleElevationMap(int scaleFactor) {

	std::vector<float> heightsTemp;

	for (int z = 0; z < elevationMapSize; z++) {

		for (int i = 0; i < scaleFactor; i++) {

			for (int x = 0; x < elevationMapSize; x++) {

				for (int k = 0; k < scaleFactor; k++) {

					float height = heights[z * elevationMapSize + x] * scaleFactor;
					heightsTemp.push_back(height);
				}
			}
		}
	}

	heights = heightsTemp;
	elevationMapSize *= scaleFactor;
}

//void TerrainGenerator::smoothFilterCreation(double** GKernel, int size) {
//
//	GKernel[0][0] = (float)1 / 9;
//	GKernel[0][1] = (float)1 / 9;
//	GKernel[0][2] = (float)1 / 9;
//
//	GKernel[1][0] = (float)1 / 9;
//	GKernel[1][1] = (float)1 / 9;
//	GKernel[1][2] = (float)1 / 9;
//
//	GKernel[2][0] = (float)1 / 9;
//	GKernel[2][1] = (float)1 / 9;
//	GKernel[2][2] = (float)1 / 9;
//
//}

// Function to create Gaussian filter. Ref: https://www.geeksforgeeks.org/gaussian-filter-generation-c/
void TerrainGenerator::gaussianFilterCreation(double** GKernel, int size) {

	// initialising standard deviation to 1.0
	double sigma = 1.0;
	double r, s = 2.0 * sigma * sigma;

	// sum is for normalization
	double sum = 0.0;

	// generating 5x5 kernel
	for (int x = -(size / 2); x <= (size / 2); x++) {
		for (int y = -(size / 2); y <= (size / 2); y++) {
			r = sqrt(x * x + y * y);
			GKernel[x + (size / 2)][y + (size / 2)] = (exp(-(r * r) / s)) / (PI * s);
			sum += GKernel[x + (size / 2)][y + (size / 2)];
		}
	}

	// normalising the Kernel
	for (int i = 0; i < size; ++i)
		for (int j = 0; j < size; ++j)
			GKernel[i][j] /= sum;
}

// dim 0: clipmaps 1: lods 2: vertices -- lod levels 240, 120, 60, 30, 15
void TerrainGenerator::createTerrainMesh(std::vector<TerrainChunk>& chunks, std::vector<std::vector<unsigned short>>& indices) {

	const int lodMaxLevel = 4;

	glm::vec3 normal(0.f, 0.f, 0.f);
	glm::vec4 col(1.f, 0.f, 0.f, 0.f);

	float maxSize = pow(2, lodMaxLevel) * 15;
	float distBetweenVertices = 0.25f;

	for (int i = 0; i < clipmaps; i++) {

		for (int j = 0; j < clipmaps; j++) {

			TerrainChunk chunk;
			for (int m = 0; m <= lodMaxLevel; m++) {

				std::vector<TerrainVertex> vertices;
				int level = pow(2, lodMaxLevel - m) * 15;
				for (int z = 0; z <= level; z++) {

					for (int x = 0; x <= level; x++) {

						float xPos = x * pow(2, m) + j * maxSize;
						float zPos = z * pow(2, m) + i * maxSize;

						//float elevation = perlin.octave2D_01(xPos * scale, zPos * scale, 5) * 30.f;
						//heights.push_back(elevation);
						glm::vec3 pos(xPos, heights[zPos * elevationMapSize + xPos], zPos);

						glm::vec2 uv((float)x / level, (float)z / level);
						vertices.push_back(TerrainVertex(pos, normal, uv, col));
					}
				}
				chunk.lods.push_back(vertices);
			}
			chunks.push_back(chunk);
		}
	}

	for (int m = 0; m <= lodMaxLevel; m++) {

		std::vector<unsigned short> indiceList;
		int level = 15 * pow(2, lodMaxLevel - m);
		for (int i = 0; i < level; i++) {

			for (int j = 0; j < level; j++) {

				indiceList.push_back((level + 1) * i + j);
				indiceList.push_back((level + 1) * (i + 1) + j);
				indiceList.push_back((level + 1) * i + j + 1);
				indiceList.push_back((level + 1) * i + j + 1);
				indiceList.push_back((level + 1) * (i + 1) + j);
				indiceList.push_back((level + 1) * (i + 1) + j + 1);
			}
		}
		indices.push_back(indiceList);
	}
}

void TerrainGenerator::erode() {

	int numIterations = 200000;
	//float initialWaterVolume = 1.f;
	float initialSpeed = 1.f;
	//int erosionRadius = 3;
	float inertia = .1f; // At zero, water will instantly change direction to flow downhill. At 1, water will never change direction. 
	float sedimentCapacityFactor = 1; // Multiplier for how much sediment a droplet can carry
	float minSlope = .01f; // Used to prevent carry capacity getting too close to zero on flatter terrain
	float erodeSpeed = .3f;
	float depositSpeed = .3f;
	//float evaporateSpeed = .02f;
	//float gravity = 10;
	int maxDropletLifetime = 16;

	for (int iteration = 0; iteration < numIterations; iteration++) {
		// Create water droplet at random point on map
		float posX = Utility::getRandomFloat(0, elevationMapSize - 1);
		float posZ = Utility::getRandomFloat(0, elevationMapSize - 1);
		float dirX = 0;
		float dirZ = 0;
		float speed = initialSpeed;
		//float water = initialWaterVolume;
		float sediment = 0;

		for (int lifetime = 0; lifetime < maxDropletLifetime; lifetime++) {
			int nodeX = (int)posX;
			int nodeZ = (int)posZ;
			int dropletIndex = nodeZ * elevationMapSize + nodeX;
			// Calculate droplet's offset inside the cell (0,0) = at NW node, (1,1) = at SE node
			float cellOffsetX = posX - nodeX;
			float cellOffsetZ = posZ - nodeZ;

			// Calculate droplet's height and direction of flow with bilinear interpolation of surrounding heights
			HeightAndGradient heightAndGradient = calculateHeightAndGradient(posX, posZ);

			// Update the droplet's direction and position (move position 1 unit regardless of speed)
			dirX = (dirX * inertia - heightAndGradient.gradientX * (1 - inertia));
			dirZ = (dirZ * inertia - heightAndGradient.gradientZ * (1 - inertia));
			// Normalize direction
			float len = sqrt(dirX * dirX + dirZ * dirZ);
			if (len != 0) {
				dirX /= len;
				dirZ /= len;
			}
			posX += dirX;
			posZ += dirZ;

			// Stop simulating droplet if it's not moving or has flowed over edge of map
			if ((dirX == 0 && dirZ == 0) || posX < 0 || posX >= elevationMapSize - 1 || posZ < 0 || posZ >= elevationMapSize - 1) {
				break;
			}

			// Find the droplet's new height and calculate the deltaHeight
			float newHeight = calculateHeightAndGradient(posX, posZ).height;
			float deltaHeight = newHeight - heightAndGradient.height;

			// Calculate the droplet's sediment capacity (higher when moving fast down a slope and contains lots of water)
			//float sedimentCapacity = Math::getMax(-deltaHeight, minSlope) * speed * water * sedimentCapacityFactor;
			//float sedimentCapacity = Math::getMax(-deltaHeight * speed * water * sedimentCapacityFactor, minSlope);

			if (deltaHeight > 0 || -deltaHeight < maxHeight / 10) { // || -deltaHeight > maxHeight / 10

				break;
				/*if (sediment > 0) {

					heights[dropletIndex] += (deltaHeight / 10) * (1 - cellOffsetX) * (1 - cellOffsetZ);
					sediment -= (deltaHeight / 10) * (1 - cellOffsetX) * (1 - cellOffsetZ);

					heights[dropletIndex + 1] += (deltaHeight / 10) * cellOffsetX * (1 - cellOffsetZ);
					sediment -= (deltaHeight / 10) * cellOffsetX * (1 - cellOffsetZ);

					heights[dropletIndex + elevationMapSize] += (deltaHeight / 10) * (1 - cellOffsetX) * cellOffsetZ;
					sediment -= (deltaHeight / 10) * (1 - cellOffsetX) * cellOffsetZ;

					heights[dropletIndex + elevationMapSize + 1] += (deltaHeight / 10) * cellOffsetX * cellOffsetZ;
					sediment -= (deltaHeight / 10) * cellOffsetX * cellOffsetZ;
				}*/

				//float amountToDeposit = (deltaHeight > 0) ? Math::getMin(deltaHeight, sediment) : (sediment - sedimentCapacity) * depositSpeed;
				//sediment -= amountToDeposit;
				//break;
			}
			else {

				// Erode a fraction of the droplet's current carry capacity.
				// Clamp the erosion to the change in height so that it doesn't dig a hole in the terrain behind the droplet
				//float amountToErode = Math::getMin((sedimentCapacity - sediment) * erodeSpeed, -deltaHeight);
				//float deltaSediment = (heights[dropletIndex] < amountToErode) ? heights[dropletIndex] : amountToErode;
				//heights[dropletIndex] -= deltaSediment;
				//sediment += deltaSediment;
				// Use erosion brush to erode from all nodes inside the droplet's erosion radius
				for (int brushPointIndex = 0; brushPointIndex < erosionBrushIndices[dropletIndex].size(); brushPointIndex++) {
					int nodeIndex = erosionBrushIndices[dropletIndex][brushPointIndex];
					float weighedErodeAmount = (-deltaHeight * erosionBrushWeights[dropletIndex][brushPointIndex]) / 4;
					//float deltaSediment = (heights[nodeIndex] < weighedErodeAmount) ? heights[nodeIndex] : weighedErodeAmount;
					heights[nodeIndex] -= weighedErodeAmount;
					sediment += weighedErodeAmount;
				}
			}


			// If carrying more sediment than capacity, or if flowing uphill:
			//if (sediment > sedimentCapacity || deltaHeight > 0) {
			//	// If moving uphill (deltaHeight > 0) try fill up to the current height, otherwise deposit a fraction of the excess sediment
			//	float amountToDeposit = (deltaHeight > 0) ? Math::getMin(deltaHeight, sediment) : (sediment - sedimentCapacity) * depositSpeed;
			//	sediment -= amountToDeposit;

			//	// Add the sediment to the four nodes of the current cell using bilinear interpolation
			//	// Deposition is not distributed over a radius (like erosion) so that it can fill small pits
			//	heights[dropletIndex] += amountToDeposit * (1 - cellOffsetX) * (1 - cellOffsetZ);
			//	heights[dropletIndex + 1] += amountToDeposit * cellOffsetX * (1 - cellOffsetZ);
			//	heights[dropletIndex + elevationMapSize] += amountToDeposit * (1 - cellOffsetX) * cellOffsetZ;
			//	heights[dropletIndex + elevationMapSize + 1] += amountToDeposit * cellOffsetX * cellOffsetZ;

			//}
			//else {
			//	// Erode a fraction of the droplet's current carry capacity.
			//	// Clamp the erosion to the change in height so that it doesn't dig a hole in the terrain behind the droplet
			//	float amountToErode = Math::getMin((sedimentCapacity - sediment) * erodeSpeed, -deltaHeight);
			//	//float deltaSediment = (heights[dropletIndex] < amountToErode) ? heights[dropletIndex] : amountToErode;
			//	//heights[dropletIndex] -= deltaSediment;
			//	//sediment += deltaSediment;
			//	// Use erosion brush to erode from all nodes inside the droplet's erosion radius
			//	for (int brushPointIndex = 0; brushPointIndex < erosionBrushIndices[dropletIndex].size(); brushPointIndex++) {
			//		int nodeIndex = erosionBrushIndices[dropletIndex][brushPointIndex];
			//		float weighedErodeAmount = amountToErode * erosionBrushWeights[dropletIndex][brushPointIndex];
			//		float deltaSediment = (heights[nodeIndex] < weighedErodeAmount) ? heights[nodeIndex] : weighedErodeAmount;
			//		heights[nodeIndex] -= deltaSediment;
			//		sediment += deltaSediment;
			//	}
			//}

			// Update droplet's speed and water content
			//speed = sqrt(speed * speed + abs(deltaHeight) * gravity);
			//water *= (1 - evaporateSpeed);
		}

		//TerrainGenerator::recreateHeightField();
	}
}

// Reference: https://github.com/SebLague/Hydraulic-Erosion/blob/master/Assets/Scripts/Erosion.cs
//void TerrainGenerator::erode() {
//
//	int numIterations = 10000;
//	float initialWaterVolume = 1.f;
//	float initialSpeed = 1.f;
//	//int erosionRadius = 3;
//	float inertia = .1f; // At zero, water will instantly change direction to flow downhill. At 1, water will never change direction. 
//	float sedimentCapacityFactor = 1; // Multiplier for how much sediment a droplet can carry
//	float minSlope = .01f; // Used to prevent carry capacity getting too close to zero on flatter terrain
//	float erodeSpeed = .3f;
//	float depositSpeed = .3f;
//	float evaporateSpeed = .02f;
//	float gravity = 10;
//	int maxDropletLifetime = 16;
//
//	for (int iteration = 0; iteration < numIterations; iteration++) {
//		// Create water droplet at random point on map
//		float posX = Utility::getRandomFloat(0, elevationMapSize - 1);
//		float posZ = Utility::getRandomFloat(0, elevationMapSize - 1);
//		float dirX = 0;
//		float dirZ = 0;
//		float speed = initialSpeed;
//		float water = initialWaterVolume;
//		float sediment = 0;
//
//		for (int lifetime = 0; lifetime < maxDropletLifetime; lifetime++) {
//			int nodeX = (int)posX;
//			int nodeZ = (int)posZ;
//			int dropletIndex = nodeZ * elevationMapSize + nodeX;
//			// Calculate droplet's offset inside the cell (0,0) = at NW node, (1,1) = at SE node
//			float cellOffsetX = posX - nodeX;
//			float cellOffsetZ = posZ - nodeZ;
//
//			// Calculate droplet's height and direction of flow with bilinear interpolation of surrounding heights
//			HeightAndGradient heightAndGradient = calculateHeightAndGradient(posX, posZ);
//
//			// Update the droplet's direction and position (move position 1 unit regardless of speed)
//			dirX = (dirX * inertia - heightAndGradient.gradientX * (1 - inertia));
//			dirZ = (dirZ * inertia - heightAndGradient.gradientZ * (1 - inertia));
//			// Normalize direction
//			float len = sqrt(dirX * dirX + dirZ * dirZ);
//			if (len != 0) {
//				dirX /= len;
//				dirZ /= len;
//			}
//			posX += dirX;
//			posZ += dirZ;
//
//			// Stop simulating droplet if it's not moving or has flowed over edge of map
//			if ((dirX == 0 && dirZ == 0) || posX < 0 || posX >= elevationMapSize - 1 || posZ < 0 || posZ >= elevationMapSize - 1) {
//				break;
//			}
//
//			// Find the droplet's new height and calculate the deltaHeight
//			float newHeight = calculateHeightAndGradient(posX, posZ).height;
//			float deltaHeight = newHeight - heightAndGradient.height;
//
//			// Calculate the droplet's sediment capacity (higher when moving fast down a slope and contains lots of water)
//			float sedimentCapacity = Math::getMax(-deltaHeight, minSlope) * speed * water * sedimentCapacityFactor;
//			//float sedimentCapacity = Math::getMax(-deltaHeight * speed * water * sedimentCapacityFactor, minSlope);
//
//			// If carrying more sediment than capacity, or if flowing uphill:
//			if (sediment > sedimentCapacity || deltaHeight > 0) {
//				// If moving uphill (deltaHeight > 0) try fill up to the current height, otherwise deposit a fraction of the excess sediment
//				float amountToDeposit = (deltaHeight > 0) ? Math::getMin(deltaHeight, sediment) : (sediment - sedimentCapacity) * depositSpeed;
//				sediment -= amountToDeposit;
//
//				// Add the sediment to the four nodes of the current cell using bilinear interpolation
//				// Deposition is not distributed over a radius (like erosion) so that it can fill small pits
//				heights[dropletIndex] += amountToDeposit * (1 - cellOffsetX) * (1 - cellOffsetZ);
//				heights[dropletIndex + 1] += amountToDeposit * cellOffsetX * (1 - cellOffsetZ);
//				heights[dropletIndex + elevationMapSize] += amountToDeposit * (1 - cellOffsetX) * cellOffsetZ;
//				heights[dropletIndex + elevationMapSize + 1] += amountToDeposit * cellOffsetX * cellOffsetZ;
//
//			}
//			else {
//				// Erode a fraction of the droplet's current carry capacity.
//				// Clamp the erosion to the change in height so that it doesn't dig a hole in the terrain behind the droplet
//				float amountToErode = Math::getMin((sedimentCapacity - sediment) * erodeSpeed, -deltaHeight);
//				//float deltaSediment = (heights[dropletIndex] < amountToErode) ? heights[dropletIndex] : amountToErode;
//				//heights[dropletIndex] -= deltaSediment;
//				//sediment += deltaSediment;
//				// Use erosion brush to erode from all nodes inside the droplet's erosion radius
//				for (int brushPointIndex = 0; brushPointIndex < erosionBrushIndices[dropletIndex].size(); brushPointIndex++) {
//					int nodeIndex = erosionBrushIndices[dropletIndex][brushPointIndex];
//					float weighedErodeAmount = amountToErode * erosionBrushWeights[dropletIndex][brushPointIndex];
//					float deltaSediment = (heights[nodeIndex] < weighedErodeAmount) ? heights[nodeIndex] : weighedErodeAmount;
//					heights[nodeIndex] -= deltaSediment;
//					sediment += deltaSediment;
//				}
//			}
//
//			// Update droplet's speed and water content
//			speed = sqrt(speed * speed + abs(deltaHeight) * gravity);
//			water *= (1 - evaporateSpeed);
//		}
//
//		//TerrainGenerator::recreateHeightField();
//	}
//}

// Reference: https://github.com/SebLague/Hydraulic-Erosion/blob/master/Assets/Scripts/Erosion.cs
HeightAndGradient TerrainGenerator::calculateHeightAndGradient(float posX, float posZ) {

	int coordX = (int)posX;
	int coordZ = (int)posZ;

	// Calculate droplet's offset inside the cell (0,0) = at NW node, (1,1) = at SE node
	float x = posX - coordX;
	float z = posZ - coordZ;

	// Calculate heights of the four nodes of the droplet's cell
	int nodeIndexNW = coordZ * elevationMapSize + coordX;
	float heightNW = heights[nodeIndexNW];
	float heightNE = heights[nodeIndexNW + 1];
	float heightSW = heights[nodeIndexNW + elevationMapSize];
	float heightSE = heights[nodeIndexNW + elevationMapSize + 1];

	// Calculate droplet's direction of flow with bilinear interpolation of height difference along the edges
	float gradientX = (heightNE - heightNW) * (1 - z) + (heightSE - heightSW) * z;
	float gradientZ = (heightSW - heightNW) * (1 - x) + (heightSE - heightNE) * x;

	// Calculate height with bilinear interpolation of the heights of the nodes of the cell
	float height = heightNW * (1 - x) * (1 - z) + heightNE * x * (1 - z) + heightSW * (1 - x) * z + heightSE * x * z;

	return HeightAndGradient{ height = height, gradientX = gradientX, gradientZ = gradientZ };
}

// Reference: https://github.com/SebLague/Hydraulic-Erosion/blob/master/Assets/Scripts/Erosion.cs
void TerrainGenerator::initializeBrushIndices(int radius) {

	int* xOffsets = new int[radius * radius * 4];
	int* zOffsets = new int[radius * radius * 4];
	float* weights = new float[radius * radius * 4];
	float weightSum = 0;
	int addIndex = 0;

	for (int i = 0; i < elevationMapSize * elevationMapSize; i++) {
		int centreX = i % elevationMapSize;
		int centreZ = i / elevationMapSize;

		if (centreZ <= radius || centreZ >= elevationMapSize - radius || centreX <= radius + 1 || centreX >= elevationMapSize - radius) {
			weightSum = 0;
			addIndex = 0;
			for (int z = -radius; z <= radius; z++) {
				for (int x = -radius; x <= radius; x++) {
					float sqrDst = x * x + z * z;
					if (sqrDst < radius * radius) {
						int coordX = centreX + x;
						int coordZ = centreZ + z;

						if (coordX >= 0 && coordX < elevationMapSize && coordZ >= 0 && coordZ < elevationMapSize) {
							float weight = 1 - sqrt(sqrDst) / radius;
							weightSum += weight;
							weights[addIndex] = weight;
							xOffsets[addIndex] = x;
							zOffsets[addIndex] = z;
							addIndex++;
						}
					}
				}
			}
		}

		int numEntries = addIndex;

		std::vector<int> temp0;
		std::vector<float> temp1;
		for (int j = 0; j < numEntries; j++) {

			temp0.push_back((zOffsets[j] + centreZ) * elevationMapSize + xOffsets[j] + centreX);
			temp1.push_back(weights[j] / weightSum);
		}
		erosionBrushIndices.push_back(temp0);
		erosionBrushWeights.push_back(temp1);
	}

	delete[] xOffsets;
	delete[] zOffsets;
	delete[] weights;
}

//void TerrainGenerator::createTerrainMesh(std::vector<TerrainChunk>& chunks, std::vector<std::vector<unsigned short>>& indices) {
//
//	// temp
//	unsigned width;
//	unsigned height;
//	std::vector<unsigned char> elevations;
//	unsigned error = lodepng::decode(elevations, width, height, "resource/heightmaps/mountain.png", LodePNGColorType::LCT_GREY, 16);
//	//---
//
//
//	const int lodMaxLevel = 4;
//	const siv::PerlinNoise perlin{ seed };
//
//	glm::vec3 normal(0.f, 0.f, 0.f);
//	glm::vec4 col(1.f, 0.f, 0.f, 0.f);
//
//	float maxSize = pow(2, lodMaxLevel) * 15;
//	float distBetweenVertices = 0.25f;
//
//	for (int i = 0; i < clipmaps; i++) {
//
//		for (int j = 0; j < clipmaps; j++) {
//
//			TerrainChunk chunk;
//			for (int m = 0; m <= lodMaxLevel; m++) {
//
//				std::vector<TerrainVertex> vertices;
//				int level = pow(2, lodMaxLevel - m) * 15;
//				for (int z = 0; z <= level; z++) {
//
//					for (int x = 0; x <= level; x++) {
//
//						/*float xPos = (x * pow(2, m) + j * maxSize) * distBetweenVertices;
//						float zPos = (z * pow(2, m) + i * maxSize) * distBetweenVertices;
//						glm::vec3 pos(xPos, perlin.octave2D((double)xPos * scale, (double)zPos * scale, 5, 0.5f) * height, zPos);*/
//
//						float xPos = x * pow(2, m) + j * maxSize;
//						float zPos = z * pow(2, m) + i * maxSize;
//
//						//int updatedX = xPos * ((width - 1) / (maxSize * clipmaps));
//						//int updatedZ = zPos * ((width - 1) / (maxSize * clipmaps));
//
//						//float elevation;
//						//uint32_t flt_init = Convert::FLTSet(elevations[(updatedZ * width + updatedX) * 4],
//						//	elevations[(updatedZ * width + updatedX) * 4 + 1], elevations[(updatedZ * width + updatedX) * 4 + 2], 
//						//	elevations[(updatedZ * width + updatedX) * 4 + 3], &elevation);
//
//						float elevation = (elevations[(zPos * width + xPos) * 2] * 256 + elevations[(zPos * width + xPos) * 2 + 1]) / 150.f;
//
//						glm::vec3 pos(xPos, elevation, zPos);
//
//						glm::vec2 uv((float)x / level, (float)z / level);
//						vertices.push_back(TerrainVertex(pos, normal, uv, col));
//					}
//				}
//				chunk.lods.push_back(vertices);
//			}
//			chunks.push_back(chunk);
//		}
//	}
//
//	for (int m = 0; m <= lodMaxLevel; m++) {
//
//		std::vector<unsigned short> indiceList;
//		int level = 15 * pow(2, lodMaxLevel - m);
//		for (int i = 0; i < level; i++) {
//
//			for (int j = 0; j < level; j++) {
//
//				indiceList.push_back((level + 1) * i + j);
//				indiceList.push_back((level + 1) * (i + 1) + j);
//				indiceList.push_back((level + 1) * i + j + 1);
//				indiceList.push_back((level + 1) * i + j + 1);
//				indiceList.push_back((level + 1) * (i + 1) + j);
//				indiceList.push_back((level + 1) * (i + 1) + j + 1);
//			}
//		}
//		indices.push_back(indiceList);
//	}
//}

glm::vec4 TerrainGenerator::evaluateColor(float& slope) {

	//float angle0 = glm::radians(8.f);

	if (slope <= 25.f)
		return glm::vec4(1.f, 0.f, 0.f, 0.f);
	else
		return glm::vec4(0.f, 1.f, 0.f, 0.f);
}

float TerrainGenerator::angleBetweenTwoVectors(glm::vec3 v0, glm::vec3 v1) {

	return glm::degrees(glm::acos(glm::dot(v0, v1)));
}

// For smooth shading
// ref: https://mrl.cs.nyu.edu/~perlin/courses/fall2002ugrad/meshnormals.html
void TerrainGenerator::recalculateNormals(std::vector<TerrainChunk>& chunks, std::vector<std::vector<unsigned short>>& indices) {

	// ? draw normals
	// fix chunk edges

	for (auto& chunk : chunks) {

		for (int i = 0; i < indices.size(); i++) {

			auto& indiceList = indices[i];
			for (int k = 0; k < indiceList.size(); k += 3) {

				glm::vec3 v0(chunk.lods[i][indiceList[k]].position);
				glm::vec3 v1(chunk.lods[i][indiceList[k + 1]].position);
				glm::vec3 v2(chunk.lods[i][indiceList[k + 2]].position);

				chunk.lods[i][indiceList[k]].normal += glm::cross((v1 - v0), (v2 - v0));
				chunk.lods[i][indiceList[k + 1]].normal += glm::cross((v2 - v1), (v0 - v1));
				chunk.lods[i][indiceList[k + 2]].normal += glm::cross((v0 - v2), (v1 - v2));
			}
		}

		for (auto& lod : chunk.lods) {

			for (auto& vert : lod) {

				vert.normal = glm::normalize(vert.normal);
				float slopeAngle = angleBetweenTwoVectors(glm::vec3(0, 1, 0), vert.normal);
				vert.color = evaluateColor(slopeAngle);
			}
		}
	}
}

void TerrainGenerator::initDebugNormalLines(std::vector<TerrainChunk>& chunks, std::vector<std::vector<unsigned short>>& indices) {

	std::vector<std::vector<std::vector<glm::vec3>>> normals;

	for (auto& chunk : chunks) {

		std::vector<std::vector<glm::vec3>> lodListForChunk;
		for (auto& lod : chunk.lods) {

			std::vector<glm::vec3> normalListforLOD;
			for (auto& vert : lod) {

				normalListforLOD.push_back(vert.position);
				normalListforLOD.push_back(vert.position + vert.normal);
			}
			lodListForChunk.push_back(normalListforLOD);
		}
		normals.push_back(lodListForChunk);
	}

	for (int i = 0; i < chunks.size(); i++) {

		std::vector<unsigned int> vaoList;
		for (int j = 0; j < chunks[i].lods.size(); j++) {
		
			unsigned int VAO;
			glGenVertexArrays(1, &VAO);
			vaoList.push_back(VAO);

			unsigned int VBO;
			glGenBuffers(1, &VBO);
			glBindVertexArray(VAO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, normals[i][j].size() * sizeof(glm::vec3), &normals[i][j][0], GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
		DebugNormalVAOs.push_back(vaoList);
	}

	debugNormalProgramID = ShaderNS::loadShaders("source/shader/Line.vert", "source/shader/Line.frag");
}

int TerrainGenerator::getLODLevel(glm::vec2 camPos, int chunkIndex) {

	int x = chunkIndex % clipmaps;
	int z = chunkIndex / clipmaps;
	glm::vec2 chunkCenter(x * 240.f + 120.f, z * 240.f + 120.f);
	float dist = glm::distance(chunkCenter, camPos);

	if (dist <= 250.f)
		return 0;
	else if (dist > 250.f && dist <= 500.f)
		return 1;
	else if (dist > 500.f && dist <= 750.f)
		return 2;
	else if (dist > 750.f && dist <= 1000.f)
		return 3;

	return 4;
}

void TerrainGenerator::createHeightField(std::vector<TerrainChunk>& chunks, std::vector<std::vector<unsigned short>>& indices) {

	for (auto& chunk : chunks) {

		std::vector<unsigned int> vaoList;
		for (int i = 0; i < chunk.lods.size(); i++) {

			unsigned int VAO;
			glGenVertexArrays(1, &VAO);
			vaoList.push_back(VAO);

			unsigned int VBO;
			glGenBuffers(1, &VBO);
			unsigned int EBO;
			glGenBuffers(1, &EBO);

			glBindVertexArray(VAO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, chunk.lods[i].size() * sizeof(TerrainVertex), &chunk.lods[i][0], GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[i].size() * sizeof(unsigned short), &indices[i][0], GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void*)0);

			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void*)offsetof(TerrainVertex, normal));

			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void*)offsetof(TerrainVertex, texCoord));

			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void*)offsetof(TerrainVertex, color));

			glBindVertexArray(0);
		}
		VAOs.push_back(vaoList);
	}
}

void TerrainGenerator::draw(Camera* camera, std::vector<Transform*>& pointLightTransforms, std::vector<Transform*>& dirLightTransforms) {

	glUseProgram(programID);

	glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, &transform->model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(programID, "view"), 1, GL_FALSE, &camera->ViewMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(programID, "projection"), 1, GL_FALSE, &camera->ProjectionMatrix[0][0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, elevationMapTexture);
	glUniform1i(glGetUniformLocation(programID, "heightMap"), 0);

	glBindVertexArray(terrainVAO);
	glDrawArrays(GL_PATCHES, 0, NUM_PATCH_PTS * rez * rez);

	/*glUseProgram(mat->programID);
	glUniformMatrix4fv(glGetUniformLocation(mat->programID, "M"), 1, GL_FALSE, &transform->model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(mat->programID, "V"), 1, GL_FALSE, &camera->ViewMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(mat->programID, "P"), 1, GL_FALSE, &camera->ProjectionMatrix[0][0]);
	glUniform3fv(glGetUniformLocation(mat->programID, "camPos"), 1, &camera->position[0]);

	int dlightCounter = 0;
	int plightCounter = 0;

	for (auto const& transform : pointLightTransforms) {

		char lightCounterTxt[4];
		sprintf(lightCounterTxt, "%d", plightCounter);

		char* tempLPos = new char[25];
		strcpy(tempLPos, "pointLightPositions[");
		strcat(tempLPos, lightCounterTxt);
		strcat(tempLPos, "]\0");

		char* tempLCol = new char[25];
		strcpy(tempLCol, "pointLightColors[");
		strcat(tempLCol, lightCounterTxt);
		strcat(tempLCol, "]\0");

		char* tempLPow = new char[25];
		strcpy(tempLPow, "pointLightPowers[");
		strcat(tempLPow, lightCounterTxt);
		strcat(tempLPow, "]\0");

		glUniform3fv(glGetUniformLocation(mat->programID, tempLPos), 1, &transform->globalPosition[0]);
		glUniform3fv(glGetUniformLocation(mat->programID, tempLCol), 1, &transform->entity->getComponent<Light>()->color[0]);
		glUniform1f(glGetUniformLocation(mat->programID, tempLPow), transform->entity->getComponent<Light>()->power);

		delete tempLPos;
		delete tempLCol;
		delete tempLPow;

		plightCounter++;
	}

	for (auto const& transform : dirLightTransforms) {

		char lightCounterTxt[4];
		sprintf(lightCounterTxt, "%d", dlightCounter);

		char* tempLPos = new char[25];
		strcpy(tempLPos, "dirLightDirections[");
		strcat(tempLPos, lightCounterTxt);
		strcat(tempLPos, "]\0");

		char* tempLCol = new char[25];
		strcpy(tempLCol, "dirLightColors[");
		strcat(tempLCol, lightCounterTxt);
		strcat(tempLCol, "]\0");

		char* tempLPow = new char[25];
		strcpy(tempLPow, "dirLightPowers[");
		strcat(tempLPow, lightCounterTxt);
		strcat(tempLPow, "]\0");

		glm::vec3 direction(-cos(transform->localRotation.x / 180.f) * sin(transform->localRotation.y / 180.f), -sin(transform->localRotation.x / 180.f),
			-cos(transform->localRotation.x / 180.f) * cos(transform->localRotation.y / 180.f));
		glUniform3fv(glGetUniformLocation(mat->programID, tempLPos), 1, &direction[0]);
		glUniform3fv(glGetUniformLocation(mat->programID, tempLCol), 1, &transform->entity->getComponent<Light>()->color[0]);
		glUniform1f(glGetUniformLocation(mat->programID, tempLPow), transform->entity->getComponent<Light>()->power);

		delete tempLPos;
		delete tempLCol;
		delete tempLPow;

		dlightCounter++;
	}

	for (int i = 0; i < mat->textureUnits.size(); i++) {

		char str[16];
		sprintf(str, "texture%d\0", i);

		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, mat->textureUnits[i]);
		glUniform1i(glGetUniformLocation(mat->programID, str), i);
	}

	for (int i = 0; i < mat->floatUnits.size(); i++) {

		char str[16];
		sprintf(str, "float%d\0", i);
		glUniform1f(glGetUniformLocation(mat->programID, str), mat->floatUnits[i]);
	}

	unsigned int indiceCounts[5] = { 345600, 86400, 21600, 5400, 1350 };
	unsigned int indiceCountsDebugNormals[5] = { 116162, 29282, 7442, 1922, 578 };

	for (int i = 0; i < VAOs.size(); i++) {

		int lodLevel = TerrainGenerator::getLODLevel(camera->position, i);
		glBindVertexArray(VAOs[i][lodLevel]);
		glDrawElements(GL_TRIANGLES, indiceCounts[lodLevel], GL_UNSIGNED_SHORT, (void*)0);
		glBindVertexArray(0);
	}*/


	 //DEBUG

	//glUseProgram(debugNormalProgramID);
	//glm::mat4 MVP = camera->ProjectionMatrix * camera->ViewMatrix;
	//glUniformMatrix4fv(glGetUniformLocation(debugNormalProgramID, "MVP"), 1, GL_FALSE, &MVP[0][0]);
	//glUniform3fv(glGetUniformLocation(debugNormalProgramID, "color"), 1, &glm::vec3(1.f, 0.0f, 0.1f)[0]);

	//for (int i = 0; i < DebugNormalVAOs.size(); i++) {

	//	int lodLevel = TerrainGenerator::getLODLevel(camera->position, i);
	//	glBindVertexArray(DebugNormalVAOs[i][lodLevel]);
	//	glDrawArrays(GL_LINES, 0, indiceCountsDebugNormals[lodLevel]);
	//	glBindVertexArray(0);
	//}

}