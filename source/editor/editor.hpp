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
#include "input.hpp"

class Editor {

private:

public:

	Render render;
	Window window;
	Camera editorCamera;
	FileSystem fileSystem;
	EditorGUI editorGUI;
	Scene scene;

	Editor();

	void startEditorScreen();

	void stayOpen();

	void drawAllAxis();

	void setRender(Render render);

	void setWindow(Window window);

	void setEditorCamera(Camera editorCamera);
};