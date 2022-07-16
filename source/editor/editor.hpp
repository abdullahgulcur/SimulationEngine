#pragma once

#include <iostream>

#include <scene.hpp>
#include "filesystem.hpp"
#include "render.hpp"
#include "window.hpp"
#include "scenecamera.hpp"
#include "editor_gui.hpp"
#include "model.hpp"
#include "texture.hpp"
#include "input.hpp"
#include "physics.hpp"
#include "gizmo.hpp"


class Editor {

private:

public:

	Window* window = NULL;
	Render* render = NULL;
	SceneCamera* sceneCamera = NULL;
	FileSystem* fileSystem = NULL;
	EditorGUI* editorGUI = NULL;
	Scene* scene = NULL;
	Physics* physics = NULL;
	SaveLoadSystem* saveLoadSystem = NULL;
	Gizmo* gizmo = NULL;

	float time = 0;
	bool gameStarted = false;

	Editor();

	void run();

};