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

class Scene {

private:

public:

	std::string name;

	Transform* rootTransform;
	std::map<int, Entity> entities;
	std::map<int, std::vector<int>> initialSceneGraph;

	Scene();

	void initSceneGraph();

	void generateSceneStructure(Transform* transform);

	void generateSceneGraph();

	bool readAllEntities();

	bool subEntityCheck(Transform* child, Transform* parent);

	bool subEntityAndItselfCheck(Transform* child, Transform* parent);

	void moveEntity(int toBeMoved, int moveTo);

	void deleteEntityFromTree(Transform* parent, int id);

	void deleteEntityCompletely(int id);

	void duplicateEntity(int id);

	void cloneEntityRecursively(Transform* base, Transform* copied);

	void newEntity(int parentID, const char* name);

	void renameEntity(int id, const char* newName);

	void saveSceneGraph();

	void writeSceneGraphFileRecursively(std::queue<Transform*> entQueue, std::ostringstream& fileTextStream);

};