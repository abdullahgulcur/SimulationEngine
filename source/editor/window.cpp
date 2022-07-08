#include "editor.hpp"
#include "window.hpp"

#include <iostream>

bool Window::dragAndDropFromOutside;
std::vector<std::string> Window::dragAndDropFiles;

Window::Window(Editor* editor) {

	Window::init();
}

void Window::init() {

	Window::startGLFW();
	Window::startGLOptions();
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

	monitor = glfwGetPrimaryMonitor();
	mode = glfwGetVideoMode(monitor);
	GLFW_window = glfwCreateWindow(mode->width, mode->height, "Fury", NULL, NULL);

	glfwMaximizeWindow(GLFW_window);
	glfwMakeContextCurrent(GLFW_window);
}

void Window::startGLOptions() {

	glfwSetInputMode(GLFW_window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(GLFW_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	glfwSetDropCallback(GLFW_window, drop_callback);

	Window::loadTitleBarIcon();

	glfwPollEvents();
}

void Window::loadTitleBarIcon() {

	GLFWimage image;
	unsigned width;
	unsigned height;
	image.pixels = TextureNS::loadPNG("resource/icons/material.png", width, height);
	image.width = width;
	image.height = height;
	glfwSetWindowIcon(GLFW_window, 1, &image);
	delete image.pixels;
}

void Window::end() {

	glfwSwapBuffers(GLFW_window);
	glfwPollEvents();
}

void Window::handleCallBacks(Editor* editor) {

	if (dragAndDropFromOutside) {

		editor->fileSystem->importFiles(dragAndDropFiles, editor->editorGUI->lastClickedItemID);
		dragAndDropFiles.clear();
		dragAndDropFromOutside = false;
	}
}

bool Window::getOpen(Editor* editor) {

	bool open = glfwGetKey(GLFW_window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(GLFW_window) == 0;

	if (!open) {

		editor->saveLoadSystem->saveSceneCamera();
	}

	return open;
}

void Window::terminateGLFW() {

	glfwTerminate();
}

void Window::drop_callback(GLFWwindow* window, int count, const char** paths)
{
	for (unsigned int i = 0; i < count; ++i)
		dragAndDropFiles.push_back(std::string(paths[i]));

	dragAndDropFromOutside = true;
}

void Window::setTitle(const char* title) { 
	
	glfwSetWindowTitle(GLFW_window, title);
}