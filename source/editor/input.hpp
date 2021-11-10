#pragma once

#include "GLfW/glfw3.h"

namespace Input {

	static int control;
	static bool key_r;
	static bool key_t;
	static bool key_s;

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
}