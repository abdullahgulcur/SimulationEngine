#pragma once

#include <string>
#include <map>
#include <queue>
#include <iostream>
#include <fstream>
#include <sstream>
#include <set>

#include "rapidxml_print.hpp"
#include "rapidxml.hpp"

#include "transform.hpp"
#include "entity.hpp"
#include "component.hpp"
#include "material.hpp"
#include "mousepick.hpp"

#include "meshrenderer.hpp"
#include "light.hpp"

#include <glm/gtx/matrix_decompose.hpp>


class Editor;

class Scene {

private:

	Editor* editor;

public:

	std::string name;

	int dirLightCount = 0;
	int pointLightCount = 0;

	Transform* rootTransform;
	std::vector<Entity> entities;
	std::map<int, std::vector<int>> initialSceneGraph;

	std::vector<MeshRenderer> meshRendererComponents;
	std::vector<Light> lightComponents;

	MousePick mousepick;

	Scene();

	void initSceneGraph();

	void generateSceneGraph();

	void generateSceneGraphRecursively(Transform* transform);

	void start();

	void update();

	void setTransformsOfComponents();

	bool readSceneGraph();

	void loadEntities();

	void loadTransformAttributes();

	void loadMeshRenderers();

	void loadLights();

	bool subEntityCheck(Transform* child, Transform* parent);

	void recompileAllMaterials();

	bool subEntityAndItselfCheck(Transform* child, Transform* parent);

	void moveEntity(int toBeMoved, int moveTo);

	void getTreeIndices(Transform* transform, std::set<int, std::greater<int>>& indices);

	void deleteEntityFromTree(Transform* parent, int id);

	void deleteEntityCompletely(int id);

	void deleteEntityCompletelyRecursively(Transform* transform);

	void duplicateEntity(int id);

	void cloneEntityRecursively(Transform* base, Transform* copied);

	void cloneComponents(int base, int entID);

	Transform* newEntity(int parentID, const char* name);

	void newLight(int parentID, const char* name, LightType type);

	void renameEntity(int id, char* newName);

	void saveEditorProperties();

	void saveEntities();

	void saveMeshRenderers();

	void saveLights();

	std::string getLightType(LightType type);

	void saveTransformAttributes();

	void saveSceneGraph();

	void writeSceneGraphFileRecursively(std::queue<Transform*> entQueue, std::ostringstream& fileTextStream);

	void setEditor(Editor* editor);

};