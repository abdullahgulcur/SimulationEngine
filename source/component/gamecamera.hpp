#pragma once

#include "math.hpp"
#include "camera.hpp"
#include "GLM/glm.hpp"

class Editor;

class GameCamera : public Component, public Camera {

private:

public:

	glm::vec2 res;

	unsigned int textureBuffer;
	unsigned int FBO;

	unsigned int gizmoVAO;
	unsigned int gizmoShaderProgramID;

	GameCamera();

	void setMatrices();

	glm::vec3 getPosition();

	void createFBO();

	void createEditorGizmos(bool recreate);

	void changeFovAxis(int oldFovAxis);

	void drawEditorGizmos(glm::mat4 MVP);

};