#include "editor.hpp"

Editor::Editor() {
}

void Editor::startEditorScreen() {

	physics.init();
	window.init();
	render.init();

	scene = new Scene();
	fileSystem.init(this);
	scene->init(this);

	SaveLoadSystem::loadSceneCamera(this);
	editorGUI.init(this);
	window.frameBufferForSceneViewport();
	bcr = new BoxColliderRenderer();
	scr = new SphereColliderRenderer();
}

void Editor::run() {

	float currentTime = (float)glfwGetTime();
	float dt = currentTime - time;
	time = currentTime;

	window.handleCallBacks(this);

	glBindFramebuffer(GL_FRAMEBUFFER, window.framebuffer);

	window.clear();

	editorGUI.newFrameImGui();

	scene->update(dt);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	editorGUI.createPanels();
	editorGUI.renderImGui();

	window.end();

	editorCamera.computeMatricesFromInputs(this);
}