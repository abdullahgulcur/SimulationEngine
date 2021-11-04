#pragma once

#include <iostream>

#include <scene.hpp>
#include "filesystem.hpp"
#include "render.hpp"
#include "window.hpp"
#include "camera.hpp"
#include "editor_gui.hpp"
#include "model.hpp"
#include "texture.hpp"

class Editor {

private:

public:

	Render render;
	Window window;
	Camera editorCamera;
	FileSystem fileSystem;
	EditorGUI editorGUI;
	//std::vector<Scene> sceneList;
	Scene scene;

	Editor();

	void startEditorScreen();

	void stayOpen();

	void drawAllAxis();

	void setRender(Render render);

	//Render getRender();

	void setWindow(Window window);

	//Window getWindow();

	void setEditorCamera(Camera editorCamera);

	//EditorCamera& getEditorCamera();

	//void addScene(Scene scene);
};