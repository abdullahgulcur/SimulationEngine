#include "editor.hpp"
#include "window.hpp"

#include <iostream>

bool Window::dragAndDropFromOutside;
std::vector<std::string> Window::dragAndDropFiles;

Window::Window() {

	std::cout << "Editor started..." << std::endl;

}

int Window::startGLFW() {

	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

	monitor = glfwGetPrimaryMonitor();
	mode = glfwGetVideoMode(monitor);
	window = glfwCreateWindow(mode->width, mode->height, title, NULL, NULL);

	glfwMaximizeWindow(window);
	glfwMakeContextCurrent(window);
}

void Window::startGLOptions() {

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	glfwSetMouseButtonCallback(window, editor->getEditorCamera().mouse_button_callback);
	glfwSetKeyCallback(window, editor->getEditorCamera().key_callback);
	glfwSetScrollCallback(window, editor->getEditorCamera().scrollCallback);
	glfwSetDropCallback(window, drop_callback);

	glfwPollEvents();
	glfwSetCursorPos(window, mode->width / 2, mode->height / 2);
	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
}

void Window::clear() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::end() {

	glfwSwapBuffers(window);
	glfwPollEvents();
}

void Window::renderScreen() {


}

void Window::handleCallBacks() {

	if (dragAndDropFromOutside) {

		editor->fileSystem.importFiles(dragAndDropFiles, editor->editorGUI.lastClickedItemID);
		dragAndDropFiles.clear();
		dragAndDropFromOutside = false;
	}
}

bool Window::getOpen() {

	return glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0;
}

void Window::terminateGLFW() {

	glfwTerminate();
}

void Window::renderToTexture() {

	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	//unsigned int textureColorbuffer;
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mode->width, mode->height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mode->width, mode->height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Window::drop_callback(GLFWwindow* window, int count, const char** paths)
{
	for (unsigned int i = 0; i < count; ++i)
		dragAndDropFiles.push_back(std::string(paths[i]));

	dragAndDropFromOutside = true;
}

void Window::setWindow(GLFWwindow* window) { this->window = window; }

GLFWwindow* Window::getGLFWwindow() { return window; }

void Window::setTitle(const char* title) { this->title = title; }

const char* Window::getTitle() { return title; }

void Window::setGLFWvidmode(GLFWvidmode* mode) { this->mode = mode; }

const GLFWvidmode* Window::getGLFWvidmode() { return mode; }

void Window::setGLFWmonitor(GLFWmonitor* monitor) { this->monitor = monitor; }

GLFWmonitor* Window::getGLFWmonitor() { return monitor; }

void Window::setEditor(Editor* editor) { this->editor = editor; }

Editor* Window::getEditor() { return editor; }