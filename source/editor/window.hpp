#pragma once
#include "GLfW/glfw3.h"

#include "saveloadsystem.hpp"
#include "input.hpp"

class Editor;

class Window {

public:

	GLFWwindow* GLFW_window;
	const GLFWvidmode* mode;
	GLFWmonitor* monitor;

	unsigned int framebuffer;
	unsigned int textureColorbuffer;
	static bool dragAndDropFromOutside;
	static std::vector<std::string> dragAndDropFiles;

	Window();

	void init();

	int startGLFW();

	void startGLOptions();

	void loadTitleBarIcon();

	void clear();

	void end();

	void handleCallBacks(Editor* editor);

	bool getOpen(Editor* editor);

	void terminateGLFW();

	void frameBufferForSceneViewport();

	void setTitle(const char* title);

	static void drop_callback(GLFWwindow* window, int count, const char** paths);

};