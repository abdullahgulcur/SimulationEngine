#include "render.hpp"

#include <stdio.h>

Render::Render() {

}

int Render::startGLEW() {

	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		//glfwTerminate();
		return -1;
	}
}

void Render::setEditor(Editor* editor) { this->editor = editor; }

Editor* Render::getEditor() { return editor; }