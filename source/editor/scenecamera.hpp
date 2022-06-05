#pragma once

#include "GLfW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui/imgui.h>

#include "input.hpp"
#include "saveloadsystem.hpp"
#include "math.hpp"

class Editor;

class SceneCamera: public Camera {

private:

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

	unsigned int framebuffer;
	unsigned int textureColorbuffer;

	float horizontalAngle = 0.f;
	float verticalAngle = 0.f;
	glm::vec3 position = glm::vec3(0, 0, -5);

	SceneCamera();

	void createFBO();

	void onUpdate(Editor* editor);

	void setMatrices();

	void computeMatricesFromInputs(Editor* editor);

	void teleportMouse(glm::vec2& mousePos, float& scenePosX, float& scenePosY, float& sceneRegionX,
		float& sceneRegionY, float& offset, GLFWwindow*& window, static bool& mouseTeleport);

	glm::vec3 getPosition();
};