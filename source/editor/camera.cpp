#include "editor.hpp"
#include "camera.hpp"

Camera::Camera()
{
	horizontalRotationSpeed *= generalSpeed;
	verticalRotationSpeed *= generalSpeed;
	horizontalTranslationSpeed *= generalSpeed;
	verticalTranslationSpeed *= generalSpeed;
	scrollSpeed *= generalSpeed;

	fovY = glm::radians(60.f);
}

void Camera::computeMatricesFromInputs(Editor* editor) {

	float offset = 10;
	GLFWwindow* window = editor->window.GLFW_window;
	const GLFWvidmode* mode = editor->window.mode;

	static float lastTime = glfwGetTime();
	static bool mouseTeleport = false;
	static bool firstCycle = true;
	static bool allow = true;
	static float lastX;
	static float lastY;

	firstCycle = Input::mouseClicked(1) || Input::mouseClicked(2);

	float currentTime = glfwGetTime();
	float deltaTime = currentTime - lastTime;

	glm::vec2 mousePos = Input::getMousePosition();
	float deltaX = !mouseTeleport ? mousePos.x - lastX : 0;
	float deltaY = !mouseTeleport ?  mousePos.y - lastY : 0;

	float& scenePosX = editor->editorGUI.scenePos.x;
	float& scenePosY = editor->editorGUI.scenePos.y;
	float& sceneRegionX = editor->editorGUI.sceneRegion.x;
	float& sceneRegionY = editor->editorGUI.sceneRegion.y;

	mouseTeleport = false;

	glm::vec3 direction(cos(verticalAngle) * sin(horizontalAngle), sin(verticalAngle), cos(verticalAngle) * cos(horizontalAngle));
	glm::vec3 right = glm::vec3(sin(horizontalAngle - 3.14f / 2.0f), 0, cos(horizontalAngle - 3.14f / 2.0f));
	glm::vec3 up = glm::cross(right, direction);

	bool insideSceneView = mousePos.x > scenePosX && mousePos.x < scenePosX + sceneRegionX && mousePos.y > scenePosY && mousePos.y < scenePosY + sceneRegionY;

	if (insideSceneView) {
		
		float delta = Input::mouseScroolDelta();
		position += direction * delta * scrollSpeed;
	}

	if (firstCycle) {

		allow = insideSceneView;
		firstCycle = false;
	}

	if (allow) {

		if (Input::mouseDown(1)) {

			Camera::teleportMouse(mousePos, scenePosX, scenePosY, sceneRegionX,
				sceneRegionY, offset, window, mouseTeleport);

			horizontalAngle -= horizontalRotationSpeed * deltaX;
			verticalAngle -= verticalRotationSpeed * deltaY;
		}

		if (Input::mouseDown(2)) {

			Camera::teleportMouse(mousePos, scenePosX, scenePosY, sceneRegionX,
				sceneRegionY, offset, window, mouseTeleport);

			position -= right * deltaX * deltaTime * horizontalTranslationSpeed;
			position += up * deltaY * deltaTime * verticalTranslationSpeed;
		}

		ImGuiMouseButton btn = ImGuiPopupFlags_MouseButtonLeft || ImGuiPopupFlags_MouseButtonRight || ImGuiPopupFlags_MouseButtonMiddle;
		if (ImGui::IsMouseReleased(btn))
			allow = false;
	}

	// These parameters are same as Unity engine
	ProjectionMatrix = glm::perspective(fovY, aspectRatio, 0.1f, 10000.0f);
	ViewMatrix = glm::lookAt(position, position + direction, up);

	lastX = mousePos.x;
	lastY = mousePos.y;
	lastTime = currentTime;
}

void Camera::teleportMouse(glm::vec2& mousePos, float& scenePosX, float& scenePosY, float& sceneRegionX,
	float& sceneRegionY, float& offset, GLFWwindow*& window, static bool& mouseTeleport) {

	if (mousePos.x < scenePosX + offset) {

		glfwSetCursorPos(window, scenePosX + sceneRegionX - offset - 1, mousePos.y);
		mouseTeleport = true;
	}
	else if (mousePos.x > scenePosX + sceneRegionX - offset) {

		glfwSetCursorPos(window, scenePosX + offset + 1, mousePos.y);
		mouseTeleport = true;
	}

	if (mousePos.y < scenePosY + offset) {

		glfwSetCursorPos(window, mousePos.x, scenePosY + sceneRegionY - offset * 5); // BUGGG !!!!
		mouseTeleport = true;
	}
	else if (mousePos.y > scenePosY + sceneRegionY - offset) {

		glfwSetCursorPos(window, mousePos.x, scenePosY + offset + 1);
		mouseTeleport = true;
	}
}