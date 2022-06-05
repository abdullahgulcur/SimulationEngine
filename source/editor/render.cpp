#include "render.hpp"
#include "editor.hpp"

#include <stdio.h>

Render::Render(Editor* editor) {

	glewExperimental = true;

	if (glewInit() != GLEW_OK)
		fprintf(stderr, "Failed to initialize GLEW\n");

	//glFrontFace(GL_CCW); // change this to ccw, its default value
	//glCullFace(GL_BACK);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(0.5f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glViewport(0, 0, editor->window->mode->width, editor->window->mode->height);
}