#include "editor.hpp"

Editor::Editor() {

	physics = new Physics();
	window = new Window(this);
	render = new Render(this);
	sceneCamera = new SceneCamera();
	fileSystem = new FileSystem();
	scene = new Scene();
	editorGUI = new EditorGUI();

	fileSystem->init(this);
	scene->init(this);
	SaveLoadSystem::loadSceneCamera(this);
	editorGUI->init(this);
}

void Editor::run() {

	float currentTime = (float)glfwGetTime();
	float dt = currentTime - time;
	time = currentTime;

	window->handleCallBacks(this);
	editorGUI->newFrameImGui();
	scene->update(dt);
	editorGUI->createPanels();
	editorGUI->renderImGui();
	window->end();
	sceneCamera->onUpdate(this);
}