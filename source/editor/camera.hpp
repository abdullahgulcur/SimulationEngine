#pragma once

#include "GLfW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui/imgui.h>

#include "input.hpp"
#include "saveloadsystem.hpp"

class Editor;

class Camera {

private:

	float horizontalRotationSpeed = 0.012;
	float verticalRotationSpeed = 0.012;
	float horizontalTranslationSpeed = 50;
	float verticalTranslationSpeed = 50;
	float scrollSpeed = 5;

	float generalSpeed = 0.15f;

public:

	float horizontalAngle = 0.f;
	float verticalAngle = 0.f;

	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;
	glm::vec3 position = glm::vec3(0, 0, -5);

	Camera();

	void computeMatricesFromInputs(Editor* editor);

	//void setCameraAngles(float horizontalAngle, float verticalAngle, float aspectRatio);

	void teleportMouse(glm::vec2& mousePos, float& scenePosX, float& scenePosY, float& sceneRegionX,
		float& sceneRegionY, float& offset, GLFWwindow*& window, static bool& mouseTeleport);
};