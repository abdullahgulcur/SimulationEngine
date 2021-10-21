#include "editor_camera.hpp"

int EditorCamera::count;
bool EditorCamera::leftClicked;
bool EditorCamera::middleClicked;
bool EditorCamera::rightClicked;
float EditorCamera::deltaX;
float EditorCamera::deltaY;
float EditorCamera::scrollYOffset;

EditorCamera::EditorCamera()
{
	horizontalRotationSpeed *= generalSpeed;
	verticalRotationSpeed *= generalSpeed;
	horizontalTranslationSpeed *= generalSpeed;
	verticalTranslationSpeed *= generalSpeed;
	scrollSpeed *= generalSpeed;

	setAspectRatio(1920, 1080);
}

EditorCamera::EditorCamera(GLFWwindow* window, const GLFWvidmode* mode)
{
	this->window = window;
	this->mode = mode;

	setBoundaries();
	setAspectRatio(1920, 1080);
}

void EditorCamera::setWindow(GLFWwindow* window) {

	this->window = window;
}

void EditorCamera::setMode(const GLFWvidmode* mode) {

	this->mode = mode;

	setBoundaries();
}

void EditorCamera::setBoundaries() {

	sceneStartPosX = 0;
	sceneStartPosY = 0;
	sceneEndPosX = mode->width;
	sceneEndPosY = mode->height;
}

void EditorCamera::setAspectRatio(float width, float height) {

	aspectRatio = width / height;
}

void EditorCamera::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		EditorCamera::leftClicked = true;
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		EditorCamera::rightClicked = true;
	}

	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
		EditorCamera::middleClicked = true;
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		EditorCamera::count = 0;
		EditorCamera::leftClicked = false;
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
		EditorCamera::count = 0;
		EditorCamera::rightClicked = false;
	}

	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE) {
		EditorCamera::count = 0;
		EditorCamera::middleClicked = false;
	}

}

void EditorCamera::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

}

void EditorCamera::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	EditorCamera::scrollYOffset = yoffset;
}

void EditorCamera::computeMatricesFromInputs() {

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

	EditorCamera::deltaX = xpos - lastX;
	EditorCamera::deltaY = ypos - lastY;

	static bool allowed = true;

	if (EditorCamera::rightClicked) {

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

			horizontalAngle -= horizontalRotationSpeed * EditorCamera::deltaX;
			verticalAngle -= verticalRotationSpeed * EditorCamera::deltaY;
		}
		EditorCamera::count++;
	}

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);

	position += direction * EditorCamera::scrollYOffset * scrollSpeed;
	EditorCamera::scrollYOffset = 0;

	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f)
	);

	// Up vector
	glm::vec3 up = glm::cross(right, direction);

	if (EditorCamera::middleClicked) {

		if (EditorCamera::count == 0) {

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
			position -= right * EditorCamera::deltaX * deltaTime * horizontalTranslationSpeed;

			// Vertical movement
			position += up * EditorCamera::deltaY * deltaTime * verticalTranslationSpeed;
		}

		EditorCamera::count++;
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

	if (EditorCamera::rightClicked && xpos >= sceneEndPosX - 15) {
		lastX = sceneStartPosX;
		flag = false;
	}
	if (EditorCamera::rightClicked && xpos < sceneStartPosX) {
		lastX = sceneEndPosX - 16;
		flag = false;
	}
	else if (flag)
		lastX = xpos;

	if (EditorCamera::rightClicked && ypos >= sceneEndPosY - 35) {
		lastY = sceneStartPosY;
		flag = false;
	}
	else if (EditorCamera::rightClicked && ypos < sceneStartPosY) {
		lastY = sceneEndPosY - 36;
		flag = false;
	}
	else if (flag)
		lastY = ypos;

	if (EditorCamera::middleClicked && xpos >= sceneEndPosX - 15) {
		lastX = sceneStartPosX;
		flag = false;
	}
	if (EditorCamera::middleClicked && xpos < sceneStartPosX) {
		lastX = sceneEndPosX - 16;
		flag = false;
	}
	else if (flag)
		lastX = xpos;

	if (EditorCamera::middleClicked && ypos >= sceneEndPosY - 35) {
		lastY = sceneStartPosY;
		flag = false;
	}
	else if (EditorCamera::middleClicked && ypos < sceneStartPosY) {
		lastY = sceneEndPosY - 36;
		flag = false;
	}
	else if (flag)
		lastY = ypos;
}