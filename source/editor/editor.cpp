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
	SaveLoadSystem::loadSceneCamera(this);
	scene.setEditor(this);
	scene.initSceneGraph();

	window.setEditor(this);
	render.setEditor(this);
	editorGUI.setEditor(this);
	editorGUI.setFiles(&fileSystem.files);

	editorGUI.initImGui();
	window.frameBufferForSceneViewport();

	scene.start();
}

void Editor::stayOpen() {

	window.handleCallBacks();

	glBindFramebuffer(GL_FRAMEBUFFER, window.framebuffer);

	window.clear();

	editorGUI.newFrameImGui();

	scene.update();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	editorGUI.createPanels();
	editorGUI.renderImGui();

	double x = 0;
	double y = 0;
	glfwGetCursorPos(window.GLFW_window, &x, &y);

	window.end();

	editorCamera.computeMatricesFromInputs(this);
}

void Editor::setRender(Render render) { this->render = render; }

void Editor::setWindow(Window window) { this->window = window; }

void Editor::setEditorCamera(Camera editorCamera) { this->editorCamera = editorCamera; }