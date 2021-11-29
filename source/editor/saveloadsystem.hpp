#pragma once

#include <string>
#include <queue>
#include <iostream>
#include <fstream>
#include <sstream>

#include "rapidxml_print.hpp"
#include "rapidxml.hpp"

#include "transform.hpp"


class Editor;
class Camera;

namespace SaveLoadSystem {

	bool loadSceneGraph(Editor* editor);

	bool saveSceneGraph(Editor* editor);

	bool writeSceneGraphFileRecursively(std::queue<Transform*> entQueue, std::ostringstream& fileTextStream);

	bool loadEntities(Editor* editor);

	bool saveEntities(Editor* editor);
	
	bool saveSceneCamera(Editor* editor);

	bool loadSceneCamera(Editor* editor);

	bool saveTransforms(Editor* editor);

	bool loadTransforms(Editor* editor);

	bool saveLights(Editor* editor);

	bool loadLights(Editor* editor);

	bool saveMeshRenderers(Editor* editor);

	bool loadMeshRenderers(Editor* editor);

	bool savePhysicsComponents(Editor* editor);

	bool loadPhysicsComponents(Editor* editor);

}