#pragma once

#include "GLfW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {

private:

	float horizontalAngle = 0.f;
	float verticalAngle = 0.0f;
	float initialFoV = 45.0f;
	float aspectRatio;

	float horizontalRotationSpeed = 0.012;
	float verticalRotationSpeed = 0.012;
	float horizontalTranslationSpeed = 50;
	float verticalTranslationSpeed = 50;
	float scrollSpeed = 5;

	float sceneStartPosX;
	float sceneStartPosY;
	float sceneEndPosX;
	float sceneEndPosY;

	float generalSpeed = 0.1f;

public:

	static int count;
	static bool leftClicked;
	static bool middleClicked;
	static bool rightClicked;
	static float deltaX;
	static float deltaY;
	static float scrollYOffset;

	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;
	glm::vec3 position = glm::vec3(0, 0, -5);
	
	GLFWwindow* window;
	const GLFWvidmode* mode;

	Camera();

	Camera(GLFWwindow* window, const GLFWvidmode* mode);

	void setWindow(GLFWwindow* window);

	void setMode(const GLFWvidmode* mode);

	void setBoundaries();

	void setAspectRatio(float width, float height);

	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

	static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

	void computeMatricesFromInputs();

};