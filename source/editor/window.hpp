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

	void renderToTexture();

	void setWindow(GLFWwindow* window);

	//GLFWwindow* getGLFWwindow();

	void setTitle(const char* title);

	//const char* getTitle();

	void setGLFWvidmode(GLFWvidmode* mode);

	//const GLFWvidmode* getGLFWvidmode();

	void setGLFWmonitor(GLFWmonitor* monitor);

	//GLFWmonitor* getGLFWmonitor();

	void setEditor(Editor* editor);

	//Editor* getEditor();

	static void drop_callback(GLFWwindow* window, int count, const char** paths);

};