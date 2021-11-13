#pragma once

#include "shader.hpp"
#include "camera.hpp"

#include <GLFW/glfw3.h>

class Editor;

class MousePick {

public:

	unsigned int pickingProgramID;
	unsigned int pickingColorID;

	unsigned int modelMatrixID;
	unsigned int viewMatrixID;
	unsigned int projectionMatrixID;

	void init();

	void detect(Editor* editor, float x, float y, float width, float height, float mouseX, float mouseY);
};