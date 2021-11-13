#pragma once
#include "GLfW/glfw3.h"

#include "input.hpp"

class Editor;

class Window {

private:

	Editor* editor;

public:

	GLFWwindow* GLFW_window;
	const char* title;
	const GLFWvidmode* mode;
	GLFWmonitor* monitor;

	unsigned int framebuffer;
	unsigned int textureColorbuffer;
	static bool dragAndDropFromOutside;
	static std::vector<std::string> dragAndDropFiles;

	Window();

	int startGLFW();

	void startGLOptions();

	void loadTitleBarIcon();

	void clear();

	void end();

	void handleCallBacks();

	bool getOpen();

	void terminateGLFW();

	void frameBufferForSceneViewport();

	void setWindow(GLFWwindow* window);

	void setTitle(const char* title);

	void setGLFWvidmode(GLFWvidmode* mode);

	void setGLFWmonitor(GLFWmonitor* monitor);

	void setEditor(Editor* editor);

	static void drop_callback(GLFWwindow* window, int count, const char** paths);

};