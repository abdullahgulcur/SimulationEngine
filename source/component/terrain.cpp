#include "terrain.hpp"
#include "camera.hpp"

TerrainGenerator::TerrainGenerator() {

}

void TerrainGenerator::init() {

	std::vector<TerrainChunk> chunks;
	std::vector<std::vector<unsigned short>> indices;
	TerrainGenerator::createTerrainMesh(chunks, indices);
	TerrainGenerator::recalculateNormals(chunks, indices);

	// Debug Purpose Only
	TerrainGenerator::initDebugNormalLines(chunks, indices);

	TerrainGenerator::createHeightField(chunks, indices);
}

void TerrainGenerator::init(MaterialFile* mat) {

	this->mat = mat;
	std::vector<TerrainChunk> chunks;
	std::vector<std::vector<unsigned short>> indices;
	TerrainGenerator::createTerrainMesh(chunks, indices);
	TerrainGenerator::recalculateNormals(chunks, indices);

	// Debug Purpose Only
	TerrainGenerator::initDebugNormalLines(chunks, indices);

	TerrainGenerator::createHeightField(chunks, indices);
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

// dim 0: clipmaps 1: lods 2: vertices -- lod levels 240, 120, 60, 30, 15
void TerrainGenerator::createTerrainMesh(std::vector<TerrainChunk>& chunks, std::vector<std::vector<unsigned short>>& indices) {

	const int lodMaxLevel = 4;
	const siv::PerlinNoise perlin{ seed };

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

						float xPos = (x * pow(2, m) + j * maxSize) * distBetweenVertices;
						float zPos = (z * pow(2, m) + i * maxSize) * distBetweenVertices;
						glm::vec3 pos(xPos, perlin.octave2D((double)xPos * scale, (double)zPos * scale, 5, 0.5f) * height, zPos);
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

glm::vec4 TerrainGenerator::evaluateColor(float& slope) {

	//float angle0 = glm::radians(8.f);

	if (slope <= 15.f)
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

int TerrainGenerator::getLODLevel(glm::vec3 camPos, int chunkIndex) {

	int x = chunkIndex % clipmaps;
	int z = chunkIndex / clipmaps;
	glm::vec3 chunkCenter(x * 60.f + 30.f, 0.f, z * 60.f + 30.f);
	float dist = glm::distance(chunkCenter, camPos);

	if (dist <= 30.f)
		return 0;
	else if (dist > 30.f && dist <= 60.f)
		return 1;
	else if (dist > 60.f && dist <= 120.f)
		return 2;
	else if (dist > 120.f && dist <= 240.f)
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

	glUseProgram(mat->programID);
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
	}


	// DEBUG

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