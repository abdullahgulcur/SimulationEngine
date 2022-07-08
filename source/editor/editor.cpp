#include "editor.hpp"

Editor::Editor() {

	physics = new Physics();
	window = new Window(this);
	render = new Render(this);
	fileSystem = new FileSystem(this);
	scene = new Scene(this);
	editorGUI = new EditorGUI(this);
	sceneCamera = new SceneCamera(this);
	saveLoadSystem = new SaveLoadSystem(this);

	fileSystem->init();
	scene->init();
	saveLoadSystem->loadSceneCamera();
	editorGUI->init();
}

void Editor::run() {

	float currentTime = (float)glfwGetTime();
	float dt = currentTime - time;
	time = currentTime;

	window->handleCallBacks(this);
	scene->update(dt);
	sceneCamera->update();
	editorGUI->update();
	window->end();
}