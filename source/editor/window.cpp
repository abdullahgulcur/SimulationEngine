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
	GLFW_window = glfwCreateWindow(mode->width, mode->height, title, NULL, NULL);

	glfwMaximizeWindow(GLFW_window);
	glfwMakeContextCurrent(GLFW_window);
}

void Window::startGLOptions() {

	glfwSetInputMode(GLFW_window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(GLFW_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	//glfwSetMouseButtonCallback(GLFW_window, editor->editorCamera.mouse_button_callback);
	//glfwSetKeyCallback(GLFW_window, editor->editorCamera.key_callback);
	//glfwSetScrollCallback(GLFW_window, editor->editorCamera.scrollCallback);
	glfwSetDropCallback(GLFW_window, drop_callback);

	Window::loadTitleBarIcon();

	glfwPollEvents();
	glfwSetCursorPos(GLFW_window, mode->width / 2, mode->height / 2);
	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glViewport(0, 0, mode->width, mode->height);
}

void Window::loadTitleBarIcon() {

	Texture texture;
	GLFWimage image;
	unsigned width;
	unsigned height;
	image.pixels = texture.loadPNG("resource/icons/material.png", width, height);
	image.width = width;
	image.height = height;
	glfwSetWindowIcon(GLFW_window, 1, &image);
	delete image.pixels;
}

void Window::clear() {

	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::end() {

	glfwSwapBuffers(GLFW_window);
	glfwPollEvents();
}

void Window::handleCallBacks() {

	if (dragAndDropFromOutside) {

		editor->fileSystem.importFiles(dragAndDropFiles, editor->editorGUI.lastClickedItemID);
		dragAndDropFiles.clear();
		dragAndDropFromOutside = false;
	}
}

bool Window::getOpen() {

	bool open = glfwGetKey(GLFW_window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(GLFW_window) == 0;

	if (!open) {

		SaveLoadSystem::saveSceneCamera(editor);
	}

	return open;
}

void Window::terminateGLFW() {

	glfwTerminate();
}

void Window::frameBufferForSceneViewport() {

	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mode->width, mode->height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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

void Window::setWindow(GLFWwindow* window) { this->GLFW_window = window; }

void Window::setTitle(const char* title) { this->title = title; }

void Window::setGLFWvidmode(GLFWvidmode* mode) { this->mode = mode; }

void Window::setGLFWmonitor(GLFWmonitor* monitor) { this->monitor = monitor; }

void Window::setEditor(Editor* editor) { this->editor = editor; }