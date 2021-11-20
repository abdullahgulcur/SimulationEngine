#include "editor.hpp"

Editor::Editor() {
}

void Editor::startEditorScreen() {

	window.init();
	render.init();
	fileSystem.init(this);
	SaveLoadSystem::loadSceneCamera(this);
	scene.init(this);
	editorGUI.init(this);
	window.frameBufferForSceneViewport();

	scene.start();
}

void Editor::run() {

	window.handleCallBacks(this);

	glBindFramebuffer(GL_FRAMEBUFFER, window.framebuffer);

	window.clear();

	editorGUI.newFrameImGui();

	scene.update();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	editorGUI.createPanels();
	editorGUI.renderImGui();

	window.end();

	editorCamera.computeMatricesFromInputs(this);
}