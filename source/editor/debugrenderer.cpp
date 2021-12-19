#include "debugrenderer.hpp"

ColliderRenderer::ColliderRenderer() {

	color = glm::vec3(0.f, 1.f, 0.f);
}

void ColliderRenderer::drawCollider(glm::mat4 MVP) {

	glUseProgram(shaderProgramID);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "MVP"), 1, GL_FALSE, &MVP[0][0]);
	glUniform3fv(glGetUniformLocation(shaderProgramID, "color"), 1, &color[0]);

	glBindVertexArray(VAO);
	glDrawArrays(GL_LINES, 0, count);
	glBindVertexArray(0);
}

//----------

BoxColliderRenderer::BoxColliderRenderer() {

	std::vector<glm::vec3> points;

	points.push_back(glm::vec3(-0.5f, -0.5f, -0.5f));
	points.push_back(glm::vec3(0.5f, -0.5f, -0.5f));
	points.push_back(glm::vec3(-0.5f, -0.5f, 0.5f));
	points.push_back(glm::vec3(0.5f, -0.5f, 0.5f));
	points.push_back(glm::vec3(-0.5f, -0.5f, -0.5f));
	points.push_back(glm::vec3(-0.5f, -0.5f, 0.5f));
	points.push_back(glm::vec3(0.5f, -0.5f, -0.5f));
	points.push_back(glm::vec3(0.5f, -0.5f, 0.5f));

	points.push_back(glm::vec3(-0.5f, 0.5f, -0.5f));
	points.push_back(glm::vec3(0.5f, 0.5f, -0.5f));
	points.push_back(glm::vec3(-0.5f, 0.5f, 0.5f));
	points.push_back(glm::vec3(0.5f, 0.5f, 0.5f));
	points.push_back(glm::vec3(-0.5f, 0.5f, -0.5f));
	points.push_back(glm::vec3(-0.5f, 0.5f, 0.5f));
	points.push_back(glm::vec3(0.5f, 0.5f, -0.5f));
	points.push_back(glm::vec3(0.5f, 0.5f, 0.5f));

	points.push_back(glm::vec3(-0.5f, -0.5f, -0.5f));
	points.push_back(glm::vec3(-0.5f, 0.5f, -0.5f));
	points.push_back(glm::vec3(0.5f, -0.5f, -0.5f));
	points.push_back(glm::vec3(0.5f, 0.5f, -0.5f));
	points.push_back(glm::vec3(-0.5f, -0.5f, 0.5f));
	points.push_back(glm::vec3(-0.5f, 0.5f, 0.5f));
	points.push_back(glm::vec3(0.5f, -0.5f, 0.5f));
	points.push_back(glm::vec3(0.5f, 0.5f, 0.5f));

	count = points.size();

	shaderProgramID = ShaderNS::loadShaders("source/shader/Line.vert", "source/shader/Line.frag");

	glGenVertexArrays(1, &VAO);
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), &points[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

SphereColliderRenderer::SphereColliderRenderer() {

	shaderProgramID = ShaderNS::loadShaders("source/shader/Circle.vert", "source/shader/Circle.frag");

	float vertices[] = {
		// positions           // texture coords
		 0.5f,  0.5f, 0.0f,    1.0f, 1.0f, // top right
		 0.5f, -0.5f, 0.0f,    1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,    0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,    0.0f, 1.0f  // top left 
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3,  // second triangle
		3, 1, 0, // first triangle
		3, 2, 1  // second triangle
	};
	count = sizeof(indices);

	unsigned int VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void SphereColliderRenderer::drawCollider(glm::mat4 MVP) {

	glUseProgram(shaderProgramID);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "MVP"), 1, GL_FALSE, &MVP[0][0]);
	glUniform3fv(glGetUniformLocation(shaderProgramID, "color"), 1, &color[0]);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, (void*)0);
	glBindVertexArray(0);
}