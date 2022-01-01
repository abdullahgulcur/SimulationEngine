#include "terrain.hpp"
#include "camera.hpp"

Terrain::Terrain() {

}

void Terrain::init(MaterialFile* mat) {

    this->mat = mat;
    std::vector<TerrainVertex> vertices;
    std::vector<unsigned short> indices;
    Terrain::createTerrainMesh(indices, vertices);
	Terrain::recalculateNormals(indices, vertices);
    Terrain::createHeightField(indices, vertices);
    indiceCount = indices.size();
}

void Terrain::recreateHeightField() {

    glDeleteVertexArrays(1, &VAO);
    std::vector<TerrainVertex> vertices;
    std::vector<unsigned short> indices;
    Terrain::createTerrainMesh(indices, vertices);
	Terrain::recalculateNormals(indices, vertices);
    Terrain::createHeightField(indices, vertices);
    indiceCount = indices.size();
}

void Terrain::createTerrainMesh(std::vector<unsigned short>& indices, std::vector<TerrainVertex>& vertices) {

    const siv::PerlinNoise perlin{ seed };

    glm::vec3 normal(0.f, 1.f, 0.f);

	for (int i = 0; i < viewportLevel_Z; i++) {

		for (int j = 0; j < viewportLevel_X; j++) {

            glm::vec3 pos(j * (size_X / viewportLevel_X),
                perlin.noise2D_01((double)j * scale, (double)i * scale) * height, i * (size_Z / viewportLevel_Z));
            glm::vec2 uv((float)j / (viewportLevel_X - 1), (float)i / (viewportLevel_Z - 1));
            glm::vec4 col(1.f, 0.f, 0.f, 0.f);
            vertices.push_back(TerrainVertex(pos, normal, uv, col));

            if (i != viewportLevel_Z - 1 && j != viewportLevel_X - 1) {

                indices.push_back(viewportLevel_X * i + j);
                indices.push_back(viewportLevel_X * (i + 1) + j);
                indices.push_back(viewportLevel_X * i + j + 1);
                indices.push_back(viewportLevel_X * i + j + 1);
                indices.push_back(viewportLevel_X * (i + 1) + j);
                indices.push_back(viewportLevel_X * (i + 1) + j + 1);
            }
		}
	}
}

glm::vec4 Terrain::evaluateColor(float& slope) {

    float angle0 = glm::radians(5.f);

    if (slope <= angle0)
        return glm::vec4(1.f, 0.f, 0.f, 0.f);
    else
        return glm::vec4(0.f, 1.f, 0.f, 0.f);
}

float Terrain::angleBetweenTwoVectors(glm::vec3 v0, glm::vec3 v1) {

	return glm::acos(glm::dot(v0, v1) / (glm::length(v0) * glm::length(v1)));
}

// For smooth shading
// ref: https://mrl.cs.nyu.edu/~perlin/courses/fall2002ugrad/meshnormals.html
void Terrain::recalculateNormals(std::vector<unsigned short>& indices, std::vector<TerrainVertex>& vertices) {

    for (int i = 0; i < indices.size(); i += 3) {

        glm::vec3 v0(vertices[indices[i]].position);
        glm::vec3 v1(vertices[indices[i + 1]].position);
        glm::vec3 v2(vertices[indices[i + 2]].position);

        vertices[indices[i]].normal += glm::cross((v1 - v0), (v2 - v0));
        vertices[indices[i + 1]].normal += glm::cross((v2 - v1), (v0 - v1));
        vertices[indices[i + 2]].normal += glm::cross((v0 - v2), (v1 - v2));
    }

	for (auto& vert : vertices) {

		vert.normal = glm::normalize(vert.normal);
		float slopeAngle = angleBetweenTwoVectors(glm::vec3(0, 1, 0), vert.normal);
		vert.color = evaluateColor(slopeAngle);
	}
}

void Terrain::createHeightField(std::vector<unsigned short>& indices, std::vector<TerrainVertex>& vertices) {

    glGenVertexArrays(1, &VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    unsigned int EBO;
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(TerrainVertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

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

void Terrain::draw(Camera* camera, std::vector<Transform*>& pointLightTransforms, std::vector<Transform*>& dirLightTransforms) {

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
		glUniform1f(glGetUniformLocation(mat->programID, str),mat->floatUnits[i]);
	}

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indiceCount, GL_UNSIGNED_SHORT, (void*)0);
	glBindVertexArray(0);
}