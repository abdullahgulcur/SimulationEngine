#include "editor.hpp"
#include "gamecamera.hpp"

GameCamera::GameCamera() {

	fov = 60.f;
	GameCamera::createEditorGizmos(false);
}

void GameCamera::init(int sizeX, int sizeY) {

	GameCamera::createFBO(sizeX, sizeY);
}

glm::vec3 GameCamera::getPosition() {

	return transform->globalPosition;
}

void GameCamera::setMatrices() {

	float verticalFOV = 0;

	if (fovAxis == 0) {
		verticalFOV = glm::radians(fov);
	}
	else {
		verticalFOV = Math::getVerticalFOV(glm::radians(fov), aspectRatio);
	}

	direction = transform->model * glm::vec4(0,0,1,0);
	glm::vec3 right = transform->model * glm::vec4(-1, 0, 0, 0);
	glm::vec3 up = glm::cross(right, direction);

	ProjectionMatrix = glm::perspective(verticalFOV, aspectRatio, nearClip, farClip);
	ViewMatrix = glm::lookAt(transform->globalPosition, transform->globalPosition + direction, up);
	projectionViewMatrix = ProjectionMatrix * ViewMatrix;
	GameCamera::frustum(projectionViewMatrix);
}

void GameCamera::createFBO(int sizeX, int sizeY) {

	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &textureBuffer);
	glBindTexture(GL_TEXTURE_2D, textureBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sizeX, sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureBuffer, 0);

	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, sizeX, sizeY);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GameCamera::recreateFBO(int sizeX, int sizeY) {

	glDeleteRenderbuffers(1, &RBO);
	glDeleteTextures(1, &textureBuffer);
	glDeleteFramebuffers(1, &FBO);

	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	//glViewport(0, 0, sizeX, sizeY);

	glGenTextures(1, &textureBuffer);
	glBindTexture(GL_TEXTURE_2D, textureBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sizeX, sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureBuffer, 0);

	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, sizeX, sizeY);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	GameCamera::updateProjectionMatrix(sizeX, sizeY);
}

void GameCamera::updateProjectionMatrix(int sizeX, int sizeY) {

	aspectRatio = (float)sizeX / sizeY;
	ProjectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
}

void GameCamera::createEditorGizmos(bool recreate) {

	if(recreate)
		glDeleteVertexArrays(1, &gizmoVAO);
	else
		gizmoShaderProgramID = ShaderNS::loadShaders("source/shader/Line.vert", "source/shader/Line.frag");

	float horizontalFOV = 0;
	float verticalFOV = 0;

	if (fovAxis == 0) {
		verticalFOV = glm::radians(fov);
		horizontalFOV = Math::getHorizontalFOV(glm::radians(fov), aspectRatio);
	}
	else {
		verticalFOV = Math::getVerticalFOV(glm::radians(fov), aspectRatio);
		horizontalFOV = glm::radians(fov);
	}

	float xn = glm::tan(horizontalFOV / 2) * nearClip;
	float yn = glm::tan(verticalFOV / 2) * nearClip;
	float xf = glm::tan(horizontalFOV / 2) * farClip;
	float yf = glm::tan(verticalFOV / 2) * farClip;

	std::vector<glm::vec3> points;

	points.push_back(glm::vec3(xn, yn, nearClip));
	points.push_back(glm::vec3(-xn, yn, nearClip));
	points.push_back(glm::vec3(-xn, yn, nearClip));
	points.push_back(glm::vec3(-xn, -yn, nearClip));
	points.push_back(glm::vec3(-xn, -yn, nearClip));
	points.push_back(glm::vec3(xn, -yn, nearClip));
	points.push_back(glm::vec3(xn, -yn, nearClip));
	points.push_back(glm::vec3(xn, yn, nearClip));

	points.push_back(glm::vec3(xf, yf, farClip));
	points.push_back(glm::vec3(-xf, yf, farClip));
	points.push_back(glm::vec3(-xf, yf, farClip));
	points.push_back(glm::vec3(-xf, -yf, farClip));
	points.push_back(glm::vec3(-xf, -yf, farClip));
	points.push_back(glm::vec3(xf, -yf, farClip));
	points.push_back(glm::vec3(xf, -yf, farClip));
	points.push_back(glm::vec3(xf, yf, farClip));

	points.push_back(glm::vec3(xn, yn, nearClip));
	points.push_back(glm::vec3(xf, yf, farClip));
	points.push_back(glm::vec3(-xn, yn, nearClip));
	points.push_back(glm::vec3(-xf, yf, farClip));
	points.push_back(glm::vec3(xn, -yn, nearClip));
	points.push_back(glm::vec3(xf, -yf, farClip));
	points.push_back(glm::vec3(-xn, -yn, nearClip));
	points.push_back(glm::vec3(-xf, -yf, farClip));


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

void GameCamera::changeFovAxis(int oldFovAxis) {

	if (fovAxis == oldFovAxis)
		return;

	fov = oldFovAxis == 0 ? glm::degrees(Math::getHorizontalFOV(glm::radians(fov), aspectRatio))
		: glm::degrees(Math::getVerticalFOV(glm::radians(fov), aspectRatio));
}

void GameCamera::drawEditorGizmos(glm::mat4 MVP) {

	glUseProgram(gizmoShaderProgramID);
	glUniformMatrix4fv(glGetUniformLocation(gizmoShaderProgramID, "MVP"), 1, GL_FALSE, &MVP[0][0]);
	glUniform3fv(glGetUniformLocation(gizmoShaderProgramID, "color"), 1, &glm::vec3(0.2f,0.2f,1)[0]);

	glBindVertexArray(gizmoVAO);
	glDrawArrays(GL_LINES, 0, 24);
	glBindVertexArray(0);
}