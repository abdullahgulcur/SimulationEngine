#include "scene.hpp"

Scene::Scene() {

	name = "MyScene";
}

void Scene::initSceneGraph() {

	sceneGraph = new Entity;

	if (!Scene::readAllEntities()) {

		SceneObject obj;
		obj.name = "Root";
		obj.addr = sceneGraph;
		entities[0] = obj;
	}

	entities[entities.begin()->first].addr = sceneGraph;
	sceneGraph->id = entities.begin()->first;
	sceneGraph->parent = NULL;

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

	std::ifstream file("resource/backup/temp_scene_graph.txt");

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

bool Scene::readAllEntities() {

	std::ifstream theFile("resource/backup/temp_entities.xml");

	if (theFile.fail())
		return false;

	rapidxml::xml_document<> doc;
	rapidxml::xml_node<>* root_node = NULL;

	// Read the sample.xml file
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

	return true;
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

	Entity* entity = new Entity;
	entity->id = std::prev(entities.end())->first + 1;
	entity->parent = entities[id].addr->parent;
	(entity->parent->childs).push_back(entity);

	SceneObject obj;
	obj.name = entities[id].name + "_copy";
	obj.addr = entity;
	entities[entity->id] = obj;

	cloneEntityRecursively(entities[id].addr, entity);
}

void Scene::cloneEntityRecursively(Entity* base, Entity* copied) {

	for (int i = 0; i < base->childs.size(); i++) {

		Entity* entity = new Entity;
		entity->id = std::prev(entities.end())->first + 1;
		entity->parent = copied;
		(entity->parent->childs).push_back(entity);

		SceneObject obj;
		obj.name = entities[base->childs[i]->id].name;
		obj.addr = entity;
		entities[entity->id] = obj;

		Scene::cloneEntityRecursively(base->childs[i], entity);
	}
}

void Scene::newEntity(int currentEntityID, const char* entityName){

	Entity* entity = new Entity;
	entity->id = std::prev(entities.end())->first + 1;
	entity->parent = entities[currentEntityID].addr;
	(entity->parent->childs).push_back(entity);

	SceneObject obj;
	obj.name = entityName;
	obj.addr = entity;
	entities[entity->id] = obj;
}

void Scene::renameEntity(int id, const char* newName) {

	entities[id].name = newName;
}

void Scene::saveSceneGraph() {

	rapidxml::xml_document<> doc;
	rapidxml::xml_node<>* decl = doc.allocate_node(rapidxml::node_declaration);
	decl->append_attribute(doc.allocate_attribute("version", "1.0"));
	decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
	doc.append_node(decl);

	rapidxml::xml_node<>* entitiesNode = doc.allocate_node(rapidxml::node_element, "Entities");
	doc.append_node(entitiesNode);
	int count = 0;
	for (std::map<int, SceneObject>::iterator it = entities.begin(); it != entities.end(); ++it) {

		rapidxml::xml_node<>* entity = doc.allocate_node(rapidxml::node_element, "Entity");
		entitiesNode->append_node(entity);

		rapidxml::xml_node<>* ent_prop_name = doc.allocate_node(rapidxml::node_element, "Name");
		rapidxml::xml_node<>* ent_prop_id = doc.allocate_node(rapidxml::node_element, "ID");

		std::string temp = std::to_string(it->second.addr->id);
		const char* node_name = doc.allocate_string(temp.c_str());

		ent_prop_id->value(node_name);
		entity->append_node(ent_prop_id);

		ent_prop_name->value(it->second.name.c_str());
		entity->append_node(ent_prop_name);
		count++;
	}

	std::string xml_as_string;
	rapidxml::print(std::back_inserter(xml_as_string), doc);

	std::ofstream file_stored("resource/backup/temp_entities.xml");
	file_stored << doc;
	file_stored.close();
	doc.clear();

	std::ofstream MyFile("resource/backup/temp_scene_graph.txt");

	std::ostringstream fileTextStream;

	std::queue<Entity*> entQueue;
	entQueue.push(sceneGraph);
	Scene::writeSceneGraphFileRecursively(entQueue, fileTextStream);

	MyFile << fileTextStream.str();
	MyFile.close();
} 

void Scene::writeSceneGraphFileRecursively(std::queue<Entity*> entQueue, std::ostringstream& fileTextStream) {

	if (entQueue.size() == 0)
		return;

	Entity* entity = entQueue.front();
	entQueue.pop();

	fileTextStream << entity->id << ' ';

	for (int i = 0; i < entity->childs.size(); i++) {

		fileTextStream << entity->childs[i]->id << ' ';
		entQueue.push(entity->childs[i]);
	}
	fileTextStream << '\n';
	Scene::writeSceneGraphFileRecursively(entQueue, fileTextStream);
}