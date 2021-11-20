#pragma once

#include "GLfW/glfw3.h"
#include "GLM/glm.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

namespace Input {

	static bool mouseAnyButtonDown;

	bool keyPressed(char keyCode);

	bool keyDown(char keyCode);

	bool keyReleased(char keyCode);

	bool mouseClicked(int button);

	bool mouseDown(int button);

	bool mouseDoubleClicked(int button);

	bool mouseReleased(int button);

	float mouseScroolDelta();

	glm::vec2 getMousePosition();

	void setMousePosition(glm::vec2 mousePos);

}