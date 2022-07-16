#include "editor.hpp"
#include "scenecamera.hpp"

SceneCamera::SceneCamera(Editor* editor)
{
	this->editor = editor;
}

void SceneCamera::init(int sizeX, int sizeY) {

	horizontalRotationSpeed *= generalSpeed;
	verticalRotationSpeed *= generalSpeed;
	horizontalTranslationSpeed *= generalSpeed;
	verticalTranslationSpeed *= generalSpeed;
	scrollSpeed *= generalSpeed;
	fov = 60;

	glm::vec3 direction(cos(verticalAngle) * sin(horizontalAngle), sin(verticalAngle), cos(verticalAngle) * cos(horizontalAngle));
	glm::vec3 right = glm::vec3(sin(horizontalAngle - 3.14f / 2.0f), 0, cos(horizontalAngle - 3.14f / 2.0f));
	glm::vec3 up = glm::cross(right, direction);
	ProjectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 100000.0f);
	ViewMatrix = glm::lookAt(position, position + direction, up);
	projectionViewMatrix = ProjectionMatrix * ViewMatrix;
	SceneCamera::frustum(projectionViewMatrix);

	SceneCamera::createFBO(sizeX, sizeY);
}

void SceneCamera::createFBO(int sizeX, int sizeY) {

	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &textureBuffer);
	glBindTexture(GL_TEXTURE_2D, textureBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sizeX, sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureBuffer, 0);

	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, sizeX, sizeY);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneCamera::recreateFBO(int sizeX, int sizeY) {

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

	SceneCamera::updateProjectionMatrix(sizeX, sizeY);
}

void SceneCamera::updateProjectionMatrix(int sizeX, int sizeY) {

	aspectRatio = editor->editorGUI->sceneRegion.x / editor->editorGUI->sceneRegion.y;
	ProjectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
}

void SceneCamera::update() {

	SceneCamera::computeMatricesFromInputs();
}

void SceneCamera::computeMatricesFromInputs() {

	float offset = 10;
	GLFWwindow* window = editor->window->GLFW_window;
	const GLFWvidmode* mode = editor->window->mode;

	firstCycle = Input::mouseClicked(1) || Input::mouseClicked(2);
	float currentTime = glfwGetTime();
	float deltaTime = currentTime - lastTime;
	glm::vec2 mousePos = Input::getMousePosition();
	float deltaX = !mouseTeleport ? mousePos.x - lastX : 0;
	float deltaY = !mouseTeleport ?  mousePos.y - lastY : 0;

	float& scenePosX = editor->editorGUI->scenePos.x;
	float& scenePosY = editor->editorGUI->scenePos.y;
	float& sceneRegionX = editor->editorGUI->sceneRegion.x;
	float& sceneRegionY = editor->editorGUI->sceneRegion.y;

	mouseTeleport = false;

	direction = glm::vec3(cos(verticalAngle) * sin(horizontalAngle), sin(verticalAngle), cos(verticalAngle) * cos(horizontalAngle));
	glm::vec3 right = glm::vec3(sin(horizontalAngle - 3.14f / 2.0f), 0, cos(horizontalAngle - 3.14f / 2.0f));
	glm::vec3 up = glm::cross(right, direction);

	bool insideSceneView = mousePos.x > scenePosX && mousePos.x < scenePosX + sceneRegionX && mousePos.y > scenePosY && mousePos.y < scenePosY + sceneRegionY;

	float mouseWheelDelta = 0;
	if (insideSceneView && !ImGui::GetIO().KeyCtrl && !ImGui::GetIO().KeyShift) {
		
		mouseWheelDelta = Input::mouseScroolDelta();

		if (mouseWheelDelta != 0) {

			int x = 0;
		}

		position += direction * mouseWheelDelta * scrollSpeed;
	}

	if (firstCycle) {

		allow = insideSceneView;
		firstCycle = false;
	}

	if (allow) {

		if (Input::mouseDown(1)) {

			SceneCamera::teleportMouse(mousePos, scenePosX, scenePosY, sceneRegionX,
				sceneRegionY, offset, window, mouseTeleport);

			horizontalAngle -= horizontalRotationSpeed * deltaX;
			verticalAngle -= verticalRotationSpeed * deltaY;
		}

		if (Input::mouseDown(2)) {

			SceneCamera::teleportMouse(mousePos, scenePosX, scenePosY, sceneRegionX,
				sceneRegionY, offset, window, mouseTeleport);

			position -= right * deltaX * deltaTime * horizontalTranslationSpeed;
			position += up * deltaY * deltaTime * verticalTranslationSpeed;
		}

		ImGuiMouseButton btn = ImGuiPopupFlags_MouseButtonLeft || ImGuiPopupFlags_MouseButtonRight || ImGuiPopupFlags_MouseButtonMiddle;
		if (ImGui::IsMouseReleased(btn))
			allow = false;
	}

	if (allow || mouseWheelDelta != 0) {

		//ProjectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 100000.0f);
		ViewMatrix = glm::lookAt(position, position + direction, up);
	}

	projectionViewMatrix = ProjectionMatrix * ViewMatrix;
	SceneCamera::frustum(projectionViewMatrix);

	lastX = mousePos.x;
	lastY = mousePos.y;
	lastTime = currentTime;
}

void SceneCamera::teleportMouse(glm::vec2& mousePos, float& scenePosX, float& scenePosY, float& sceneRegionX,
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

glm::vec3 SceneCamera::getPosition() {

	return position;
}