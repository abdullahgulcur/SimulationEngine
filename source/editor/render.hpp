#pragma once

#include <GL/glew.h>

class Editor;

class Render {

public:

	Render(Editor* editor);

	int init();
};