#pragma once

#include <iostream>

#include <scene.hpp>
#include "filesystem.hpp"
#include "render.hpp"
#include "window.hpp"
#include "editor_camera.hpp"
#include "editor_gui.hpp"
#include "model.hpp"
#include "texture.hpp"

class Editor {

private:

	Render render;
	Window window;

public:

	EditorCamera editorCamera;
	FileSystem fileSystem;
	EditorGUI editorGUI;
	//std::vector<Scene> sceneList;
	Scene scene;

	Editor();

	void startEditorScreen();

	void stayOpen();

	void drawAllAxis();

	void setRender(Render render);

	Render getRender();

	void setWindow(Window window);

	Window getWindow();

	void setEditorCamera(EditorCamera editorCamera);

	EditorCamera& getEditorCamera();

	//void addScene(Scene scene);
};