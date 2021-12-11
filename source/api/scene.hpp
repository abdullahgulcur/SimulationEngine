#pragma once

#include <string>
#include <map>
#include <queue>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_set>

#include "rapidxml_print.hpp"
#include "rapidxml.hpp"

#include "transform.hpp"
#include "entity.hpp"
#include "material.hpp"
#include "mousepick.hpp"

#include "meshrenderer.hpp"
#include "light.hpp"
#include "rigidbody.hpp"
#include "collider.hpp"
#include "component.hpp"

#include <glm/gtx/matrix_decompose.hpp>

class Editor;

class Scene {

private:

	Editor* editor;

public:

	std::string name;

	std::vector<Entity> entities;
	std::vector<Transform*> tempTransforms;

	std::vector<Transform*> pointLightTransforms;
	std::vector<Transform*> dirLightTransforms;

	MousePick mousepick;

	Scene();

	void init(Editor* editor);

	void initSceneGraph();

	void generateSceneGraph(std::map<int, std::vector<int>>& initialSceneGraph);

	void generateSceneGraphRecursively(Transform* parent, std::map<int, std::vector<int>>& initialSceneGraph);

	void loadPhysicsComponents();

	void loadLights();

	void start();

	void update(float dt);

	void simulateInEditor(float dt);

	//void simulateInGame(float dt);

	void setTransformsOfComponents();

	bool subEntityCheck(Transform* child, Transform* parent);

	void recompileAllMaterials();

	bool subEntityAndItselfCheck(Transform* child, Transform* parent);

	void moveEntity(int toBeMoved, int moveTo);

	void getTreeIndices(Transform* transform, std::unordered_set<int>& indices);

	void deleteEntityFromTree(Transform* parent, int id);

	void deleteEntityCompletely(int id);

	void deleteEntityCompletelyRecursively(Transform* transform);

	int duplicateEntity(int id);

	void cloneEntityRecursively(Transform* base, Transform* copied);

	Transform* newEntity(int parentID, const char* name);

	int newLight(int parentID, const char* name, LightType type);

	void renameEntity(int id, char* newName);

	void saveEditorProperties();

	std::string getLightType(LightType type);

	void writeSceneGraphFileRecursively(std::queue<Transform*> entQueue, std::ostringstream& fileTextStream);

	void setEditor(Editor* editor);

};