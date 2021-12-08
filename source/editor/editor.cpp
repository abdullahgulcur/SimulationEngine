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
	physics.init();

	scene.start();
}

void Editor::run() {

	float currentTime = (float)glfwGetTime();
	float dt = currentTime - time;
	time = currentTime;

	physics.gScene->simulate(dt);
	physics.gScene->fetchResults(true);

	window.handleCallBacks(this);

	glBindFramebuffer(GL_FRAMEBUFFER, window.framebuffer);

	window.clear();

	editorGUI.newFrameImGui();

	scene.update(dt);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	editorGUI.createPanels();
	editorGUI.renderImGui();

	window.end();

	editorCamera.computeMatricesFromInputs(this);
}