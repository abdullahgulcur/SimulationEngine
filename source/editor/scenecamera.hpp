#pragma once

#include "GLfW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "input.hpp"
#include "saveloadsystem.hpp"
#include "math.hpp"

class Editor;

class SceneCamera: public Camera {

private:

	Editor* editor;

	float horizontalRotationSpeed = 0.012;
	float verticalRotationSpeed = 0.012;
	float horizontalTranslationSpeed = 50;
	float verticalTranslationSpeed = 50;
	float scrollSpeed = 100;
	float generalSpeed = 0.15f;

	bool mouseTeleport = false;
	bool firstCycle = true;
	bool allow = true;
	float lastTime;
	float lastX;
	float lastY;

public:

	float horizontalAngle = 0.f;
	float verticalAngle = 0.f;
	glm::vec3 position = glm::vec3(0, 0, -5);

	SceneCamera(Editor* editor);

	void init(int sizeX, int sizeY);

	void createFBO(int sizeX, int sizeY);

	void update();

	void recreateFBO(int sizeX, int sizeY);

	void updateProjectionMatrix(int sizeX, int sizeY);

	void setMatrices();

	void computeMatricesFromInputs();

	void teleportMouse(glm::vec2& mousePos, float& scenePosX, float& scenePosY, float& sceneRegionX,
		float& sceneRegionY, float& offset, GLFWwindow*& window, static bool& mouseTeleport);

	glm::vec3 getPosition();
};