#pragma once

#include <string>
#include <map>
#include <queue>
#include <iostream>
#include <fstream>
#include <sstream>

#include "rapidxml_print.hpp"
#include "rapidxml.hpp"

#include "transform.hpp"
#include "entity.hpp"
#include "component.hpp"

class Editor;

class Scene {

private:

	Editor* editor;

public:

	std::string name;

	Transform* rootTransform;
	std::map<int, Entity> entities;
	std::map<int, Transform> transforms;
	std::map<int, std::vector<int>> initialSceneGraph;

	std::map<int, AnimationComponent> animationComponents;
	std::map<int, AnimatorComponent> animatorComponents;
	std::map<int, ColliderComponent> colliderComponents;
	std::map<int, LightComponent> lightComponents;
	std::map<int, MeshRendererComponent> meshRendererComponents;
	std::map<int, RigidBodyComponent> rigidBodyComponents;
	std::map<int, ScriptComponent> scriptComponents;
	std::map<int, TransformComponent> transformComponents;


	Scene();

	void initSceneGraph();

	void generateSceneGraph(Transform* transform);

	bool readSceneGraph();

	void loadEntities();

	void loadTransforms();

	void loadMeshRenderers();

	bool subEntityCheck(Transform* child, Transform* parent);

	bool subEntityAndItselfCheck(Transform* child, Transform* parent);

	void moveEntity(int toBeMoved, int moveTo);

	void deleteEntityFromTree(Transform* parent, int id);

	void deleteEntityCompletely(int id);

	void duplicateEntity(int id);

	void cloneEntityRecursively(Transform* base, Transform* copied);

	Transform* newEntity(int parentID, const char* name);

	void newPointLight(int parentID, const char* name);

	void newDirectionalLight(int parentID, const char* name);

	void renameEntity(int id, const char* newName);

	void saveEditorProperties();

	void saveEntities();

	void saveMeshRenderers();

	void saveTransforms();

	void saveSceneGraph();

	void writeSceneGraphFileRecursively(std::queue<Transform*> entQueue, std::ostringstream& fileTextStream);

	void setEditor(Editor* editor);

};