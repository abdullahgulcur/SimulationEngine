#pragma once

#include <vector>
#include <string>
#include <map>
#include <queue>

#include "scene_object.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include "rapidxml_print.hpp"
#include "rapidxml.hpp"

//using namespace std;
//using namespace rapidxml;




struct Entity {

	int id;
	Entity* parent;
	std::vector<Entity*> childs;
};

class Scene {

private:

	int index = 0;

public:

	std::string name;

	Entity* sceneGraph;
	std::map<int, SceneObject> entities;
	std::map<int, std::vector<int>> initialGraph;

	Scene();

	void initSceneGraph();

	void generateSceneStructure(Entity* sceneGraph);

	void generateSceneGraph();

	bool readAllEntities();

	bool subEntityCheck(Entity* entityToMove, Entity* entityToBeMoved);

	void moveEntity(int toBeMoved, int moveTo);

	void deleteEntityFromTree(Entity* parent, int id);

	void deleteEntityCompletely(int id);

	void duplicateEntity(int id);

	void cloneEntityRecursively(Entity* base, Entity* copied);

	void newEntity(int currentEntityID, const char* entityName);

	void renameEntity(int id, const char* newName);

	void saveSceneGraph();

	void writeSceneGraphFileRecursively(std::queue<Entity*> entQueue, std::ostringstream& fileTextStream);

};