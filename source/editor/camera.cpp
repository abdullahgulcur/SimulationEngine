#include "camera.hpp"

int Camera::count;
bool Camera::leftClicked;
bool Camera::middleClicked;
bool Camera::rightClicked;
float Camera::deltaX;
float Camera::deltaY;
float Camera::scrollYOffset;

Camera::Camera()
{
	horizontalRotationSpeed *= generalSpeed;
	verticalRotationSpeed *= generalSpeed;
	horizontalTranslationSpeed *= generalSpeed;
	verticalTranslationSpeed *= generalSpeed;
	scrollSpeed *= generalSpeed;

	setAspectRatio(1920, 1080);
}

Camera::Camera(GLFWwindow* window, const GLFWvidmode* mode)
{
	this->window = window;
	this->mode = mode;

	setBoundaries();
	setAspectRatio(1920, 1080);
}

void Camera::setWindow(GLFWwindow* window) {

	this->window = window;
}

void Camera::setMode(const GLFWvidmode* mode) {

	this->mode = mode;

	setBoundaries();
}

void Camera::setBoundaries() {

	sceneStartPosX = 0;
	sceneStartPosY = 0;
	sceneEndPosX = mode->width;
	sceneEndPosY = mode->height;
}

void Camera::setAspectRatio(float width, float height) {

	aspectRatio = width / height;
}

void Camera::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		Camera::leftClicked = true;
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		Camera::rightClicked = true;
	}

	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
		Camera::middleClicked = true;
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		Camera::count = 0;
		Camera::leftClicked = false;
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
		Camera::count = 0;
		Camera::rightClicked = false;
	}

	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE) {
		Camera::count = 0;
		Camera::middleClicked = false;
	}

}

void Camera::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

}

void Camera::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	Camera::scrollYOffset = yoffset;
}

void Camera::computeMatricesFromInputs() {

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	static float lastX;
	static float lastY;

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	double xpos, ypos;

	glfwGetCursorPos(window, &xpos, &ypos);

	Camera::deltaX = xpos - lastX;
	Camera::deltaY = ypos - lastY;

	static bool allowed = true;

	if (Camera::rightClicked) {

		if (count == 0) {

			if (xpos < sceneEndPosX + 1 && xpos > sceneStartPosX - 1 && ypos < sceneEndPosY + 1 && ypos > sceneStartPosY - 1)
				allowed = true;
			else
				allowed = false;
		}

		if (allowed) {
			if (xpos >= sceneEndPosX - 15) {
				glfwSetCursorPos(window, sceneStartPosX, ypos);
			}
			else if (xpos < sceneStartPosX) {
				glfwSetCursorPos(window, sceneEndPosX - 16, ypos);
			}

			if (ypos >= sceneEndPosY - 35) {
				glfwSetCursorPos(window, xpos, sceneStartPosY);
			}
			else if (ypos < sceneStartPosY) {
				glfwSetCursorPos(window, xpos, sceneEndPosY - 36);
			}

			horizontalAngle -= horizontalRotationSpeed * Camera::deltaX;
			verticalAngle -= verticalRotationSpeed * Camera::deltaY;
		}
		Camera::count++;
	}

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);

	position += direction * Camera::scrollYOffset * scrollSpeed;
	Camera::scrollYOffset = 0;

	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f)
	);

	// Up vector
	glm::vec3 up = glm::cross(right, direction);

	if (Camera::middleClicked) {

		if (Camera::count == 0) {

			if (xpos < sceneEndPosX + 1 && xpos > sceneStartPosX - 1 && ypos < sceneEndPosY + 1 && ypos > sceneStartPosY - 1)
				allowed = true;
			else
				allowed = false;
		}

		if (allowed) {
			if (xpos >= sceneEndPosX - 15) {
				glfwSetCursorPos(window, sceneStartPosX, ypos);
			}
			else if (xpos < sceneStartPosX) {
				glfwSetCursorPos(window, sceneEndPosX - 16, ypos);
			}

			if (ypos >= sceneEndPosY - 35) {
				glfwSetCursorPos(window, xpos, sceneStartPosY);
			}
			else if (ypos < sceneStartPosY) {
				glfwSetCursorPos(window, xpos, sceneEndPosY - 36);
			}

			// Horizontal movement
			position -= right * Camera::deltaX * deltaTime * horizontalTranslationSpeed;

			// Vertical movement
			position += up * Camera::deltaY * deltaTime * verticalTranslationSpeed;
		}

		Camera::count++;
	}

	float FoV = initialFoV; // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(glm::radians(FoV), aspectRatio, 0.1f, 100.0f);
	// Camera matrix
	ViewMatrix = glm::lookAt(
		position,           // Camera is here
		position + direction, // and looks here : at the same position, plus "direction"
		up                  // Head is up (set to 0,-1,0 to look upside-down)
	);

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;

	bool flag = true;

	if (Camera::rightClicked && xpos >= sceneEndPosX - 15) {
		lastX = sceneStartPosX;
		flag = false;
	}
	if (Camera::rightClicked && xpos < sceneStartPosX) {
		lastX = sceneEndPosX - 16;
		flag = false;
	}
	else if (flag)
		lastX = xpos;

	if (Camera::rightClicked && ypos >= sceneEndPosY - 35) {
		lastY = sceneStartPosY;
		flag = false;
	}
	else if (Camera::rightClicked && ypos < sceneStartPosY) {
		lastY = sceneEndPosY - 36;
		flag = false;
	}
	else if (flag)
		lastY = ypos;

	if (Camera::middleClicked && xpos >= sceneEndPosX - 15) {
		lastX = sceneStartPosX;
		flag = false;
	}
	if (Camera::middleClicked && xpos < sceneStartPosX) {
		lastX = sceneEndPosX - 16;
		flag = false;
	}
	else if (flag)
		lastX = xpos;

	if (Camera::middleClicked && ypos >= sceneEndPosY - 35) {
		lastY = sceneStartPosY;
		flag = false;
	}
	else if (Camera::middleClicked && ypos < sceneStartPosY) {
		lastY = sceneEndPosY - 36;
		flag = false;
	}
	else if (flag)
		lastY = ypos;
}