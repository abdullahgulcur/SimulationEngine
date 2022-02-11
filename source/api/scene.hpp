#pragma once

#include <string>
#include <map>
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

public:

	Editor* editor;

	std::string name;

	std::vector<Entity*> entities;
	std::vector<Transform*> pointLightTransforms;
	std::vector<Transform*> dirLightTransforms;

	MousePick mousepick;

	Scene();

	void init(Editor* editor);

	void initSceneGraph();

	void generateSceneGraphRecursively(int parentIndex, std::map<int, std::vector<int>>& initialSceneGraph);

	void loadLights();

	void start();

	void update(float dt);

	void simulateInEditor(float dt);

	void simulateInGame(float dt);

	void setTransformsOfComponents();

	bool subEntityCheck(Transform* child, Transform* parent);

	void deleteScene();

	void recompileAllMaterials();

	bool subEntityAndItselfCheck(Transform* child, Transform* parent);

	void moveEntity(Entity* toBeMoved, Entity* moveTo);

	void getAllEntities(Transform* transform, std::unordered_set<Entity*>& ents);

	void deleteEntityFromTree(Transform* parent, Entity* ent);

	void deleteEntityCompletely(Entity* ent);

	void deleteEntityCompletelyRecursively(Transform* transform);

	Entity* duplicateEntity(Entity* ent);

	void cloneEntityRecursively(Transform* base, Transform* copied);

	Transform* newEntity(Entity* parent, const char* name);

	int getEntityIndex(Entity* ent);

	Entity* newLight(Entity* parent, const char* name, LightType type);

	void renameEntity(Entity* ent, char* newName);

	void saveEditorProperties();

	std::string getLightType(LightType type);

	void setEditor(Editor* editor);

};