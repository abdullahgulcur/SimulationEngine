#include "editor.hpp"
#include "scene.hpp"

Scene::Scene() {

	name = "MyScene";
}

void Scene::initSceneGraph() {


	rootTransform = new Transform;

	if (!Scene::readSceneGraph()) {

		rootTransform->parent = NULL;
		rootTransform->id = 0;

		Entity entitity;
		entitity.setScene(this);
		entitity.name = "Root";
		entitity.transform = rootTransform;
		entities[rootTransform->id] = entitity;
	}
	else {

		/* LOAD ALL THE COMPONENTS */
		Scene::loadEntities();
		Scene::loadTransforms();
		Scene::loadMeshRenderers();

		Scene::generateSceneGraph(rootTransform);
		entities[rootTransform->id].transform = rootTransform;
	}
}

void Scene::generateSceneGraph(Transform* parent) {

	for (int i = 0; i < initialSceneGraph[parent->id].size(); i++) {

		Transform* transform = new Transform;
		transform->id = initialSceneGraph[parent->id][i];
		transform->parent = parent;

		transform->position = transforms[transform->id].position;
		transform->rotation = transforms[transform->id].rotation;
		transform->scale = transforms[transform->id].scale;

		(parent->children).push_back(transform);

		entities[transform->id].transform = transform;

		for (int i = 0; i < editor->fileSystem.meshes.size(); i++) {

			if (strcmp(meshRendererComponents[transform->id].meshName.c_str(), editor->fileSystem.meshes[i].name.c_str()) == 0) {

				meshRendererComponents[transform->id].renderer = &editor->fileSystem.meshes[i];
				meshRendererComponents[transform->id].transform = transform;
			}
		}

		Scene::generateSceneGraph(transform);
	}
}

bool Scene::readSceneGraph() {

	std::ifstream file("resource/backup/temp_scene_graph.txt");

	if (file.fail())
		return false;

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

void Scene::loadEntities() {

	std::ifstream file("resource/backup/temp_entities.xml");

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
		obj.setScene(this);
		obj.name = entity_node->first_node("Name")->value();
		entities[atoi(entity_node->first_node("ID")->value())] = obj;
	}
}

void Scene::loadTransforms() {

	std::ifstream file("resource/backup/temp_transforms.xml");

	rapidxml::xml_document<> doc;
	rapidxml::xml_node<>* root_node = NULL;

	// Read the sample.xml file
	std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	buffer.push_back('\0');

	// Parse the buffer
	doc.parse<0>(&buffer[0]);

	// Find out the root node
	root_node = doc.first_node("Transforms");

	// Iterate over the entity nodes
	for (rapidxml::xml_node<>* transform_node = root_node->first_node("Transform"); transform_node; transform_node = transform_node->next_sibling()) {
		
		Transform transform;

		//transform.id = atoi(transform_node->first_attribute("ID")->value());

		transform.position.x = atof(transform_node->first_node("Position")->first_attribute("X")->value());
		transform.position.y = atof(transform_node->first_node("Position")->first_attribute("Y")->value());
		transform.position.z = atof(transform_node->first_node("Position")->first_attribute("Z")->value());

		transform.rotation.x = atof(transform_node->first_node("Rotation")->first_attribute("X")->value());
		transform.rotation.y = atof(transform_node->first_node("Rotation")->first_attribute("Y")->value());
		transform.rotation.z = atof(transform_node->first_node("Rotation")->first_attribute("Z")->value());

		transform.scale.x = atof(transform_node->first_node("Scale")->first_attribute("X")->value());
		transform.scale.y = atof(transform_node->first_node("Scale")->first_attribute("Y")->value());
		transform.scale.z = atof(transform_node->first_node("Scale")->first_attribute("Z")->value());
		
		transforms[atoi(transform_node->first_attribute("ID")->value())] = transform;
	}
}

void Scene::loadMeshRenderers() {

	std::ifstream file("resource/backup/mesh_renderers.xml");

	if (file.fail())
		return;

	rapidxml::xml_document<> doc;
	rapidxml::xml_node<>* root_node = NULL;

	// Read the sample.xml file
	std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	buffer.push_back('\0');

	// Parse the buffer
	doc.parse<0>(&buffer[0]);

	// Find out the root node
	root_node = doc.first_node("MeshRenderers");

	// Iterate over the entity nodes
	for (rapidxml::xml_node<>* mesh_node = root_node->first_node("Mesh"); mesh_node; mesh_node = mesh_node->next_sibling()) {

		MeshRendererComponent comp;
		comp.id = atoi(mesh_node->first_attribute("ID")->value());
		comp.meshName = mesh_node->first_attribute("Name")->value();
		//comp.transform = entities[comp.id].transform;
		meshRendererComponents[comp.id] = comp;
		entities[comp.id].components.push_back(ComponentType::MeshRenderer);

		//entities[atoi(mesh_node->first_attribute("ID")->value())].addMeshRendererComponent(mesh_node->first_attribute("Name")->value());
		
	}
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
	obj.setScene(this);
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
		obj.setScene(this);
		obj.name = entities[base->children[i]->id].name;
		obj.transform = entity;
		entities[entity->id] = obj;

		Scene::cloneEntityRecursively(base->children[i], entity);
	}
}

Transform* Scene::newEntity(int parentID, const char* name){

	Transform* entity = new Transform;
	entity->id = std::prev(entities.end())->first + 1;
	entity->parent = entities[parentID].transform;
	(entity->parent->children).push_back(entity);

	Entity obj;
	obj.setScene(this);
	obj.name = name;
	obj.transform = entity;
	entities[entity->id] = obj;

	return entity;
}

void Scene::newPointLight(int parentID, const char* name) {

	Transform* entity = Scene::newEntity(parentID, name);

	//entity.addComponent(PointLight);
}

void Scene::newDirectionalLight(int parentID, const char* name) {

	Transform* entity = Scene::newEntity(parentID, name);

	//entity.addComponent(DirectionalLight);
}

void Scene::renameEntity(int id, const char* newName) {

	entities[id].name = newName;
}

void Scene::saveEditorProperties() {

	Scene::saveSceneGraph();
	Scene::saveEntities();
	Scene::saveTransforms();
	Scene::saveMeshRenderers();
} 

void Scene::saveEntities() {

	rapidxml::xml_document<> doc;
	rapidxml::xml_node<>* decl = doc.allocate_node(rapidxml::node_declaration);
	decl->append_attribute(doc.allocate_attribute("version", "1.0"));
	decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
	doc.append_node(decl);

	rapidxml::xml_node<>* entitiesNode = doc.allocate_node(rapidxml::node_element, "Entities");
	doc.append_node(entitiesNode);
	for (std::map<int, Entity>::iterator it = entities.begin(); it != entities.end(); ++it) {

		// this if statement is useless, but risk is reduced...
		if (it->second.transform == NULL)
			continue;

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
	}

	std::string xml_as_string;
	rapidxml::print(std::back_inserter(xml_as_string), doc);

	std::ofstream file_stored("resource/backup/temp_entities.xml");
	file_stored << doc;
	file_stored.close();
	doc.clear();
}

void Scene::saveMeshRenderers() {

	rapidxml::xml_document<> doc;
	rapidxml::xml_node<>* decl = doc.allocate_node(rapidxml::node_declaration);
	decl->append_attribute(doc.allocate_attribute("version", "1.0"));
	decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
	doc.append_node(decl);

	rapidxml::xml_node<>* meshRenderersNode = doc.allocate_node(rapidxml::node_element, "MeshRenderers");
	doc.append_node(meshRenderersNode);
	for (std::map<int, MeshRendererComponent>::iterator it = meshRendererComponents.begin(); it != meshRendererComponents.end(); ++it) {

		rapidxml::xml_node<>* meshNode = doc.allocate_node(rapidxml::node_element, "Mesh");

		std::string temp_str = std::to_string(it->second.id);
		const char* temp_val = doc.allocate_string(temp_str.c_str());

		meshNode->append_attribute(doc.allocate_attribute("ID", temp_val));

		temp_val = doc.allocate_string(meshRendererComponents[it->second.id].meshName.c_str());
		meshNode->append_attribute(doc.allocate_attribute("Name", temp_val));

		meshRenderersNode->append_node(meshNode);
	}

	std::string xml_as_string;
	rapidxml::print(std::back_inserter(xml_as_string), doc);

	std::ofstream file_stored("resource/backup/mesh_renderers.xml");
	file_stored << doc;
	file_stored.close();
	doc.clear();
}

void Scene::saveTransforms() {

	rapidxml::xml_document<> doc;
	rapidxml::xml_node<>* decl = doc.allocate_node(rapidxml::node_declaration);
	decl->append_attribute(doc.allocate_attribute("version", "1.0"));
	decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
	doc.append_node(decl);

	rapidxml::xml_node<>* transformsNode = doc.allocate_node(rapidxml::node_element, "Transforms");
	doc.append_node(transformsNode);
	for (std::map<int, Entity>::iterator it = entities.begin(); it != entities.end(); ++it) {

		// this if statement is useless, but risk is reduced...
		if (it->second.transform == NULL)
			continue;

		rapidxml::xml_node<>* transformNode = doc.allocate_node(rapidxml::node_element, "Transform");

		std::string temp_str = std::to_string(it->second.transform->id);
		const char* temp_val = doc.allocate_string(temp_str.c_str());

		transformNode->append_attribute(doc.allocate_attribute("ID", temp_val));
		transformsNode->append_node(transformNode);

		// Position

		rapidxml::xml_node<>* positionNode = doc.allocate_node(rapidxml::node_element, "Position");

		temp_str = std::to_string(it->second.transform->position.x);
		temp_val = doc.allocate_string(temp_str.c_str());

		positionNode->append_attribute(doc.allocate_attribute("X", temp_val));

		temp_str = std::to_string(it->second.transform->position.y);
		temp_val = doc.allocate_string(temp_str.c_str());

		positionNode->append_attribute(doc.allocate_attribute("Y", temp_val));

		temp_str = std::to_string(it->second.transform->position.z);
		temp_val = doc.allocate_string(temp_str.c_str());

		positionNode->append_attribute(doc.allocate_attribute("Z", temp_val));

		transformNode->append_node(positionNode);

		// Rotation

		rapidxml::xml_node<>* rotationNode = doc.allocate_node(rapidxml::node_element, "Rotation");

		temp_str = std::to_string(it->second.transform->rotation.x);
		temp_val = doc.allocate_string(temp_str.c_str());

		rotationNode->append_attribute(doc.allocate_attribute("X", temp_val));

		temp_str = std::to_string(it->second.transform->rotation.y);
		temp_val = doc.allocate_string(temp_str.c_str());

		rotationNode->append_attribute(doc.allocate_attribute("Y", temp_val));

		temp_str = std::to_string(it->second.transform->rotation.z);
		temp_val = doc.allocate_string(temp_str.c_str());

		rotationNode->append_attribute(doc.allocate_attribute("Z", temp_val));

		transformNode->append_node(rotationNode);

		// Scale

		rapidxml::xml_node<>* scaleNode = doc.allocate_node(rapidxml::node_element, "Scale");

		temp_str = std::to_string(it->second.transform->scale.x);
		temp_val = doc.allocate_string(temp_str.c_str());

		scaleNode->append_attribute(doc.allocate_attribute("X", temp_val));

		temp_str = std::to_string(it->second.transform->scale.y);
		temp_val = doc.allocate_string(temp_str.c_str());

		scaleNode->append_attribute(doc.allocate_attribute("Y", temp_val));

		temp_str = std::to_string(it->second.transform->scale.z);
		temp_val = doc.allocate_string(temp_str.c_str());

		scaleNode->append_attribute(doc.allocate_attribute("Z", temp_val));

		transformNode->append_node(scaleNode);
	}

	std::string xml_as_string;
	rapidxml::print(std::back_inserter(xml_as_string), doc);

	std::ofstream file_stored("resource/backup/temp_transforms.xml");
	file_stored << doc;
	file_stored.close();
	doc.clear();
}

void Scene::saveSceneGraph() {

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

void Scene::setEditor(Editor* editor) { this->editor = editor; }