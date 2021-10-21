#pragma once

#include <GL/glew.h>
#include "Material.hpp"

class Editor;

class Render {

private:

	Editor* editor;

public:

	Render();

	int startGLEW();

	void setEditor(Editor* editor);

	Editor* getEditor();

};