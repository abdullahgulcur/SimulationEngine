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

	Window window;
	Render render;
	Camera editorCamera;
	FileSystem fileSystem;
	EditorGUI editorGUI;
	Scene scene;

	Editor();

	void startEditorScreen();

	void run();

};