#include "scene.hpp"

Scene::Scene() {

	name = "MyScene";
}

void Scene::initSceneGraph() {

	rootTransform = new Transform;

	if (!Scene::readAllEntities()) {

		Entity entitity;
		entitity.name = "Root";
		entitity.transform = rootTransform;
		entities[0] = entitity;
	}

	entities[entities.begin()->first].transform = rootTransform;
	rootTransform->id = entities.begin()->first;
	rootTransform->parent = NULL;

	Scene::generateSceneGraph();
	Scene::generateSceneStructure(rootTransform);
}

void Scene::generateSceneStructure(Transform* parent) {

	for (int i = 0; i < initialSceneGraph[parent->id].size(); i++) {

		Transform* transform = new Transform;
		transform->id = initialSceneGraph[parent->id][i];
		transform->parent = parent;
		(parent->children).push_back(transform);

		entities[transform->id].transform = transform;

		Scene::generateSceneStructure(transform);
	}
}

void Scene::generateSceneGraph() {

	std::ifstream file("resource/backup/temp_scene_graph.txt");

	std::string transformIDs;
	while (std::getline(file, transformIDs)) {

		std::stringstream ss(transformIDs);
		std::vector<int> numbers;

		int i = 0;
		ss >> i;
		int index = i;

		for (int i = 1; ss >> i; )
			numbers.push_back(i);

		initialSceneGraph[index] = numbers;
	}

	file.close();
}

bool Scene::readAllEntities() {

	std::ifstream file("resource/backup/temp_entities.xml");

	if (file.fail())
		return false;

	rapidxml::xml_document<> doc;
	rapidxml::xml_node<>* root_node = NULL;

	// Read the sample.xml file
	std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	buffer.push_back('\0');

	// Parse the buffer
	doc.parse<0>(&buffer[0]);

	// Find out the root node
	root_node = doc.first_node("Entities");

	// Iterate over the entity nodes
	for (rapidxml::xml_node<>* entity_node = root_node->first_node("Entity"); entity_node; entity_node = entity_node->next_sibling()) {
		Entity obj;
		obj.name = entity_node->first_node("Name")->value();
		entities[atoi(entity_node->first_node("ID")->value())] = obj;
	}

	return true;
}

bool Scene::subEntityCheck(Transform* child, Transform* parent) {

	while (child->parent != NULL) {

		if (child->parent == parent)
			return true;

		child = child->parent;
	}
	return false;
}

bool Scene::subEntityAndItselfCheck(Transform* child, Transform* parent) {

	while (child->parent != NULL) {

		if (child == parent)
			return true;

		child = child->parent;
	}
	return false;
}

void Scene::moveEntity(int toBeMoved, int moveTo) {

	if (Scene::subEntityCheck(entities[moveTo].transform, entities[toBeMoved].transform))
		return;

	Transform* oldParent = entities[toBeMoved].transform->parent;
	entities[toBeMoved].transform->parent = entities[moveTo].transform;
	(entities[moveTo].transform->children).push_back(entities[toBeMoved].transform);
	Scene::deleteEntityFromTree(oldParent, toBeMoved);
}

void Scene::deleteEntityFromTree(Transform* parent, int id) {

	for (int i = 0; i < parent->children.size(); i++) {

		if (parent->children[i]->id == id) {

			parent->children.erase(parent->children.begin() + i);
			break;
		}
	}
}

void Scene::deleteEntityCompletely(int id) {

	Transform* parent = entities[id].transform->parent;
	Scene::deleteEntityFromTree(parent, id);
	entities.erase(id);
}

void Scene::duplicateEntity(int id) {

	Transform* entity = new Transform;
	entity->id = std::prev(entities.end())->first + 1;
	entity->parent = entities[id].transform->parent;
	(entity->parent->children).push_back(entity);

	Entity obj;
	obj.name = entities[id].name + "_copy";
	obj.transform = entity;
	entities[entity->id] = obj;

	cloneEntityRecursively(entities[id].transform, entity);
}

void Scene::cloneEntityRecursively(Transform* base, Transform* copied) {

	for (int i = 0; i < base->children.size(); i++) {

		Transform* entity = new Transform;
		entity->id = std::prev(entities.end())->first + 1;
		entity->parent = copied;
		(entity->parent->children).push_back(entity);

		Entity obj;
		obj.name = entities[base->children[i]->id].name;
		obj.transform = entity;
		entities[entity->id] = obj;

		Scene::cloneEntityRecursively(base->children[i], entity);
	}
}

void Scene::newEntity(int parentID, const char* name){

	Transform* entity = new Transform;
	entity->id = std::prev(entities.end())->first + 1;
	entity->parent = entities[parentID].transform;
	(entity->parent->children).push_back(entity);

	Entity obj;
	obj.name = name;
	obj.transform = entity;
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
	for (std::map<int, Entity>::iterator it = entities.begin(); it != entities.end(); ++it) {

		rapidxml::xml_node<>* entity = doc.allocate_node(rapidxml::node_element, "Entity");
		entitiesNode->append_node(entity);

		rapidxml::xml_node<>* ent_prop_name = doc.allocate_node(rapidxml::node_element, "Name");
		rapidxml::xml_node<>* ent_prop_id = doc.allocate_node(rapidxml::node_element, "ID");

		std::string temp = std::to_string(it->second.transform->id);
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

	std::queue<Transform*> entQueue;
	entQueue.push(rootTransform);
	Scene::writeSceneGraphFileRecursively(entQueue, fileTextStream);

	MyFile << fileTextStream.str();
	MyFile.close();
} 

void Scene::writeSceneGraphFileRecursively(std::queue<Transform*> entQueue, std::ostringstream& fileTextStream) {

	if (entQueue.size() == 0)
		return;

	Transform* entity = entQueue.front();
	entQueue.pop();

	fileTextStream << entity->id << ' ';

	for (int i = 0; i < entity->children.size(); i++) {

		fileTextStream << entity->children[i]->id << ' ';
		entQueue.push(entity->children[i]);
	}
	fileTextStream << '\n';
	Scene::writeSceneGraphFileRecursively(entQueue, fileTextStream);
}