#include "render.hpp"

#include <stdio.h>

Render::Render() {}

int Render::init() {

	glewExperimental = true;

	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		return - 1;
	}
}