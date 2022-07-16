#include "editor.hpp"
#include "gizmo.hpp"

Gizmo::Gizmo(Editor* editor) {

	this->editor = editor;
	Gizmo::createGizmoVAO();
}

void Gizmo::createGizmoVAO() {

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

	lineRendererProgramID = ShaderNS::loadShaders("source/shader/Line.vert", "source/shader/Line.frag");

	glGenVertexArrays(1, &cubeVAO);
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindVertexArray(cubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(glm::vec3), &points[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Gizmo::drawAABB_Box(const glm::mat4& MVP) {


	glUseProgram(lineRendererProgramID);
	glUniformMatrix4fv(glGetUniformLocation(lineRendererProgramID, "MVP"), 1, GL_FALSE, &MVP[0][0]);
	glUniform3fv(glGetUniformLocation(lineRendererProgramID, "color"), 1, &glm::vec3(1, 1, 1)[0]);

	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_LINES, 0, 24);
	glBindVertexArray(0);
}