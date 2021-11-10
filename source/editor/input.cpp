#include "input.hpp"

namespace Input {

	void Input::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

		if (key == GLFW_KEY_T && action == GLFW_PRESS)
			Input::key_t = true;

		if (key == GLFW_KEY_R && action == GLFW_PRESS)
			Input::key_r = true;

		if (key == GLFW_KEY_S && action == GLFW_PRESS)
			Input::key_s = true;

		if (key == GLFW_KEY_T && action == GLFW_RELEASE)
			Input::key_t = false;

		if (key == GLFW_KEY_R && action == GLFW_RELEASE)
			Input::key_r = false;

		if (key == GLFW_KEY_S && action == GLFW_RELEASE)
			Input::key_s = false;


		if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS)
			Input::control = true;

		if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_RELEASE)
			Input::control = false;

	}
}