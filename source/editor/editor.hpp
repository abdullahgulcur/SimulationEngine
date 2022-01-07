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
#include "physics.hpp"
#include "debugrenderer.hpp"


class Editor {

private:

public:

	Window window;
	Render render;
	Camera editorCamera;
	FileSystem fileSystem;
	EditorGUI editorGUI;
	Scene* scene = NULL;
	Scene* sceneTemp = NULL;
	Physics physics;
	BoxColliderRenderer* bcr = NULL;
	SphereColliderRenderer* scr = NULL;

	float time = 0;
	bool gameStarted = false;

	Editor();

	void startEditorScreen();

	void run();

};