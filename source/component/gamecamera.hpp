#pragma once

#include "math.hpp"
#include "camera.hpp"
#include "GLM/glm.hpp"

class Editor;

class GameCamera : public Component, public Camera {

private:

public:

	unsigned int gizmoVAO;
	unsigned int gizmoShaderProgramID;

	GameCamera();

	void setMatrices();

	glm::vec3 getPosition();

	void init(int sizeX, int sizeY);

	void createFBO(int sizeX, int sizeY);

	void recreateFBO(int sizeX, int sizeY);

	void updateProjectionMatrix(int sizeX, int sizeY);

	void createEditorGizmos(bool recreate);

	void changeFovAxis(int oldFovAxis);

	void drawEditorGizmos(glm::mat4 MVP);

};