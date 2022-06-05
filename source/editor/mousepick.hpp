#pragma once

#include "shader.hpp"
#include "scenecamera.hpp"

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

	int detectAndGetEntityId(Editor* editor, float x, float y, float width, float height, float mouseX, float mouseY);
};