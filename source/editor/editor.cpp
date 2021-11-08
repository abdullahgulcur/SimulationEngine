#include "editor.hpp"

Editor::Editor() {
}

void Editor::startEditorScreen() {

	window.setTitle("Fury");
	window.startGLFW();
	render.startGLEW();
	window.startGLOptions();
	fileSystem.initFileSystem();
	fileSystem.setEditor(this);
	scene.setEditor(this);
	scene.initSceneGraph();

	window.setEditor(this);
	render.setEditor(this);
	editorGUI.setEditor(this);
	editorGUI.setFiles(&fileSystem.files);

	editorGUI.initImGui();
	editorCamera.setWindow(window.GLFW_window);
	editorCamera.setMode(window.mode);
	window.renderToTexture();

	scene.start();
}

void Editor::stayOpen() {

	window.handleCallBacks();

	editorCamera.computeMatricesFromInputs();

	glBindFramebuffer(GL_FRAMEBUFFER, window.framebuffer);
	glViewport(0, 0, window.mode->width, window.mode->height);

	window.clear();

	editorGUI.newFrameImGui();

	scene.update();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	editorGUI.createPanels();
	editorGUI.renderImGui();

	window.end();
}

void Editor::setRender(Render render) { this->render = render; }

void Editor::setWindow(Window window) { this->window = window; }

void Editor::setEditorCamera(Camera editorCamera) { this->editorCamera = editorCamera; }