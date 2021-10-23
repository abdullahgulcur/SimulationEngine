#include "scene.hpp"

Scene::Scene() {

	name = "MyScene";
}

void Scene::initSceneGraph() {

	Scene::readAllEntities();

	sceneGraph = new Entity;
	sceneGraph->id = entities.begin()->first;
	sceneGraph->parent = NULL;
	entities[entities.begin()->first].addr = sceneGraph;

	Scene::generateSceneGraph();
	Scene::generateSceneStructure(sceneGraph);
}

void Scene::generateSceneStructure(Entity* sceneGraph) {

	for (int i = 0; i < initialGraph[sceneGraph->id].size(); i++) {

		Entity* graph = new Entity;
		graph->id = initialGraph[sceneGraph->id][i];
		graph->parent = sceneGraph;
		(sceneGraph->childs).push_back(graph);

		entities[graph->id].addr = graph;

		Scene::generateSceneStructure(graph);
	}
}

void Scene::generateSceneGraph() {

	std::ifstream file("resource/backup/scenegraph.txt");

	std::string nodeIDs;
	while (std::getline(file, nodeIDs)) {

		std::stringstream ss(nodeIDs);
		std::vector<int> numbers;

		int i = 0;
		ss >> i;
		int index = i;

		for (int i = 1; ss >> i; )
			numbers.push_back(i);

		initialGraph[index] = numbers;
	}

	file.close();
}

void Scene::readAllEntities() {

	rapidxml::xml_document<> doc;
	rapidxml::xml_node<>* root_node = NULL;

	// Read the sample.xml file
	std::ifstream theFile("resource/backup/entities.xml");
	std::vector<char> buffer((std::istreambuf_iterator<char>(theFile)), std::istreambuf_iterator<char>());
	buffer.push_back('\0');

	// Parse the buffer
	doc.parse<0>(&buffer[0]);

	// Find out the root node
	root_node = doc.first_node("Entities");

	// Iterate over the entity nodes
	for (rapidxml::xml_node<>* entity_node = root_node->first_node("Entity"); entity_node; entity_node = entity_node->next_sibling()) {
		SceneObject obj;
		obj.name = entity_node->first_node("Name")->value();
		entities[atoi(entity_node->first_node("ID")->value())] = obj;
	}
}

bool Scene::subEntityCheck(Entity* entityToMove, Entity* entityToBeMoved) {

	while (entityToMove->parent != NULL) {

		if (entityToMove->parent == entityToBeMoved)
			return true;

		entityToMove = entityToMove->parent;
	}
	return false;
}

void Scene::moveEntity(int toBeMoved, int moveTo) {

	if (Scene::subEntityCheck(entities[moveTo].addr, entities[toBeMoved].addr))
		return;

	Entity* oldParent = entities[toBeMoved].addr->parent;
	entities[toBeMoved].addr->parent = entities[moveTo].addr;
	(entities[moveTo].addr->childs).push_back(entities[toBeMoved].addr);
	Scene::deleteEntityFromTree(oldParent, toBeMoved);
}

void Scene::deleteEntityFromTree(Entity* parent, int id) {

	for (int i = 0; i < parent->childs.size(); i++) {

		if (parent->childs[i]->id == id) {

			parent->childs.erase(parent->childs.begin() + i);
			break;
		}
	}
}

void Scene::deleteEntityCompletely(int id) {

	Entity* parent = entities[id].addr->parent;
	Scene::deleteEntityFromTree(parent, id);
	entities.erase(id);
}

void Scene::duplicateEntity(int id) {

}

void Scene::newEntity(int currentEntityID, const char* entityName){

}

void Scene::renameEntity(int id, const char* newName) {

}

bool Scene::hasChild(Entity* entity) {
	return false;

}