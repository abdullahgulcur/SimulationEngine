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
		entitity.name = "Root";
		entitity.transform = rootTransform;
		entities.push_back(entitity);

	}
	else {

		Scene::loadEntities();
		Scene::loadMeshRenderers();
		//Scene::loadLights();

		entities[0].transform = rootTransform;

		Scene::generateSceneGraph(rootTransform);

		Scene::loadTransformAttributes();

		entities[rootTransform->id].transform = rootTransform;
	}

	Scene::saveEditorProperties();

}

void Scene::generateSceneGraph(Transform* parent) {

	for (int i = 0; i < initialSceneGraph[parent->id].size(); i++) {

		Transform* transform = new Transform;
		transform->id = initialSceneGraph[parent->id][i];
		transform->parent = parent;

		(parent->children).push_back(transform);

		entities[transform->id].transform = transform;

		Scene::generateSceneGraph(transform);
	}
}

void Scene::start() {

	Material mat;
	programID = mat.LoadShaders("source/shader/tri.vs", "source/shader/tri.fs");
	mvpMatrixID = glGetUniformLocation(programID, "MVP");
	glUseProgram(programID);
}

//void Scene::update() {
//
//	for (auto const& [key, val] : meshRendererComponents)
//	{
//		glm::mat4 projection = editor->editorCamera.ProjectionMatrix;
//
//		glm::mat4 view = editor->editorCamera.ViewMatrix;
//		glm::mat4 model = glm::mat4(1.0);
//		model = glm::translate(model, val.transform->position);
//		glm::mat4 mvp = projection * view * model;
//
//		glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);
//
//		glBindVertexArray(val.renderer->VAO);
//		glDrawElements(GL_TRIANGLES, val.renderer->indices.size(), GL_UNSIGNED_INT, (void*)0);
//		glBindVertexArray(0);
//	}
//}

//void Scene::setTransformsOfComponents() {
//
//	for (auto const& [key, val] : meshRendererComponents)
//		meshRendererComponents[key].transform = entities[key].transform;
//
//	for (auto const& [key, val] : lightComponents)
//		lightComponents[key].transform = entities[key].transform;
//}

bool Scene::readSceneGraph() {

	std::ifstream file(editor->fileSystem.assetsPathExternal + "\\MyProject\\Database\\scenegraph_db.txt");

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

	std::ifstream file(editor->fileSystem.assetsPathExternal + "\\MyProject\\Database\\entity_db.xml");

	rapidxml::xml_document<> doc;
	rapidxml::xml_node<>* root_node = NULL;

	std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	buffer.push_back('\0');

	doc.parse<0>(&buffer[0]);

	root_node = doc.first_node("Entities");

	for (rapidxml::xml_node<>* entity_node = root_node->first_node("Entity"); entity_node; entity_node = entity_node->next_sibling()) {
		Entity ent;
		ent.name = entity_node->first_attribute("Name")->value();
		entities.push_back(ent);
	}
}

void Scene::loadTransformAttributes() {

	std::ifstream file(editor->fileSystem.assetsPathExternal + "\\MyProject\\Database\\transform_db.xml");

	rapidxml::xml_document<> doc;
	rapidxml::xml_node<>* root_node = NULL;

	std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	buffer.push_back('\0');

	doc.parse<0>(&buffer[0]);

	root_node = doc.first_node("Transforms");

	int count = 0;
	for (rapidxml::xml_node<>* transform_node = root_node->first_node("Transform"); transform_node; transform_node = transform_node->next_sibling()) {

		entities[count].transform->position.x = atof(transform_node->first_node("Position")->first_attribute("X")->value());
		entities[count].transform->position.y = atof(transform_node->first_node("Position")->first_attribute("Y")->value());
		entities[count].transform->position.z = atof(transform_node->first_node("Position")->first_attribute("Z")->value());

		entities[count].transform->rotation.x = atof(transform_node->first_node("Rotation")->first_attribute("X")->value());
		entities[count].transform->rotation.y = atof(transform_node->first_node("Rotation")->first_attribute("Y")->value());
		entities[count].transform->rotation.z = atof(transform_node->first_node("Rotation")->first_attribute("Z")->value());

		entities[count].transform->scale.x = atof(transform_node->first_node("Scale")->first_attribute("X")->value());
		entities[count].transform->scale.y = atof(transform_node->first_node("Scale")->first_attribute("Y")->value());
		entities[count].transform->scale.z = atof(transform_node->first_node("Scale")->first_attribute("Z")->value());

		count++;
	}
}

void Scene::loadMeshRenderers() {

	std::ifstream file(editor->fileSystem.assetsPathExternal + "\\MyProject\\Database\\meshrenderer_db.xml");

	if (file.fail())
		return;

	rapidxml::xml_document<> doc;
	rapidxml::xml_node<>* root_node = NULL;

	std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	buffer.push_back('\0');

	doc.parse<0>(&buffer[0]);

	root_node = doc.first_node("MeshRendererComponents");

	for (rapidxml::xml_node<>* mesh_node = root_node->first_node("MeshRendererComponent"); mesh_node; mesh_node = mesh_node->next_sibling()) {

		MeshRenderer m_renderer;
		m_renderer.entID = atoi(mesh_node->first_attribute("EntID")->value());

		auto meshFound = editor->fileSystem.meshes.find(mesh_node->first_attribute("MeshName")->value());
		if (meshFound != editor->fileSystem.meshes.end())
			m_renderer.mesh = &meshFound->second;
		else
			m_renderer.mesh = &editor->fileSystem.meshes["Null"];

		auto matFound = editor->fileSystem.materials.find(mesh_node->first_attribute("MatName")->value());
		if (matFound != editor->fileSystem.materials.end())
			m_renderer.mat = &matFound->second;
		else
			m_renderer.mat = &editor->fileSystem.materials["Default"];

		entities[m_renderer.entID].components.push_back(ComponentType::MeshRenderer);
		meshRendererComponents.insert({ m_renderer.entID, m_renderer });
	}
}

//void Scene::loadLights() {
//
//	std::ifstream file("resource/backup/light_db.xml");
//
//	if (file.fail())
//		return;
//
//	rapidxml::xml_document<> doc;
//	rapidxml::xml_node<>* root_node = NULL;
//
//	std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
//	buffer.push_back('\0');
//
//	doc.parse<0>(&buffer[0]);
//
//	root_node = doc.first_node("Lights");
//
//	for (rapidxml::xml_node<>* light_node = root_node->first_node("Light"); light_node; light_node = light_node->next_sibling()) {
//
//		LightComponent comp;
//		comp.id = atoi(light_node->first_attribute("ID")->value());
//		comp.type = strcmp(light_node->first_attribute("Type")->value(), "Point") == 0 ? LightType::PointLight : LightType::DirectionalLight;
//		comp.power = atof(light_node->first_attribute("Power")->value());
//
//		comp.color.x = atof(light_node->first_node("Color")->first_attribute("X")->value());
//		comp.color.y = atof(light_node->first_node("Color")->first_attribute("Y")->value());
//		comp.color.z = atof(light_node->first_node("Color")->first_attribute("Z")->value());
//
//		lightComponents[comp.id] = comp;
//		entities[comp.id].components.push_back(ComponentType::Light);
//	}
//}

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

void Scene::getTreeIndices(Transform* transform, std::vector<int>& indices) {

	for (int i = 0; i < transform->children.size(); i++)
		getTreeIndices(transform->children[i], indices);

	indices.push_back(transform->id);
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

	std::vector<int> indices;
	Scene::getTreeIndices(entities[id].transform, indices);
	std::sort(indices.begin(), indices.end(), std::greater<int>());

	Transform* parent = entities[id].transform->parent;
	Scene::deleteEntityFromTree(parent, id);

	for (int i = 0; i < indices.size(); i++) {

		delete entities[indices[i]].transform;
		entities.erase(entities.begin() + indices[i]);
	}

	for (int i = indices[indices.size() - 1]; i < entities.size(); i++)
		entities[i].transform->id = i;

	//Scene::deleteEntityCompletelyRecursively(transform);
	//Scene::deleteEntityFromTreeAlternatively(transform, parent);
}

//void Scene::deleteEntityCompletelyRecursively(Transform* transform) {
//
//	for (int i = 0; i < transform->children.size(); i++) 
//		Scene::deleteEntityCompletelyRecursively(transform->children[i]);
//
//	for (int i = 0; i < entities[transform->id].components.size(); i++) {
//
//		switch (entities[transform->id].components[i]) {
//
//		case ComponentType::Light: {
//
//			lightComponents.erase(transform->id);
//			break;
//		}
//		case ComponentType::MeshRenderer: {
//
//			meshRendererComponents.erase(transform->id);
//			break;
//		}
//		}
//	}
//	entities.erase(transform->id);
//
//	delete transform;	
//}

void Scene::duplicateEntity(int id) {

	Transform* transform = new Transform;
	transform->id = entities.size();
	transform->parent = entities[id].transform->parent;
	(transform->parent->children).push_back(transform);

	Entity ent;
	ent.name = entities[id].name + "_cpy";
	ent.transform = transform;
	entities.push_back(ent);

	//for (int i = 0; i < entities[id].components.size(); i++) {

	//	switch (entities[id].components[i]) {

	//	case ComponentType::Light: {

	//		LightComponent comp = lightComponents[id];
	//		comp.transform = entity;
	//		comp.id = entity->id;
	//		lightComponents[comp.id] = comp;
	//		entities[entity->id].components.push_back(ComponentType::Light);
	//		break;
	//	}
	//	case ComponentType::MeshRenderer: {

	//		MeshRendererComponent comp = meshRendererComponents[id];
	//		comp.transform = entity;
	//		comp.id = entity->id;
	//		meshRendererComponents[comp.id] = comp;
	//		entities[entity->id].components.push_back(ComponentType::MeshRenderer);
	//		break;
	//	}	
	//	}
	//}

	cloneEntityRecursively(entities[id].transform, transform);
}

void Scene::cloneEntityRecursively(Transform* base, Transform* copied) {

	for (int i = 0; i < base->children.size(); i++) {

		Transform* transform = new Transform;
		transform->id = entities.size();
		transform->parent = copied;
		(transform->parent->children).push_back(transform);

		Entity ent;
		ent.name = entities[base->children[i]->id].name;
		ent.transform = transform;
		entities.push_back(ent);

		//for (int j = 0; j < entities[base->children[i]->id].components.size(); j++) {

		//	switch (entities[base->children[i]->id].components[j]) {

		//	case ComponentType::Light: {

		//		LightComponent comp = lightComponents[base->children[i]->id];
		//		comp.transform = transform;
		//		comp.id = transform->id;
		//		lightComponents[comp.id] = comp;
		//		entities[transform->id].components.push_back(ComponentType::Light);
		//		break;
		//	}
		//	case ComponentType::MeshRenderer: {

		//		MeshRendererComponent comp = meshRendererComponents[base->children[i]->id];
		//		comp.transform = transform;
		//		comp.id = transform->id;
		//		meshRendererComponents[comp.id] = comp;
		//		entities[transform->id].components.push_back(ComponentType::MeshRenderer);
		//		break;
		//	}
		//	}
		//}
		Scene::cloneEntityRecursively(base->children[i], transform);
	}
}

Transform* Scene::newEntity(int parentID, const char* name){

	Transform* transform = new Transform;
	transform->id = entities.size();
	transform->parent = entities[parentID].transform;
	(transform->parent->children).push_back(transform);

	Entity ent;
	ent.name = name;
	ent.transform = transform;
	entities.push_back(ent);

	return transform;
}

//void Scene::newPointLight(int parentID, const char* name) {
//
//	Transform* entity = Scene::newEntity(parentID, name);
//	entities[entity->id].addLightComponent();
//	lightComponents[entity->id].type = LightType::PointLight;
//}

//void Scene::newDirectionalLight(int parentID, const char* name) {
//
//	Transform* entity = Scene::newEntity(parentID, name);
//	entities[entity->id].addLightComponent();
//	lightComponents[entity->id].type = LightType::DirectionalLight;
//}

void Scene::renameEntity(int id, const char* newName) {

	entities[id].name = newName;
}

void Scene::saveEditorProperties() {

	Scene::saveSceneGraph();
	Scene::saveEntities();
	Scene::saveTransformAttributes();
	Scene::saveMeshRenderers();
	//Scene::saveLights();
} 

void Scene::saveEntities() {

	rapidxml::xml_document<> doc;
	rapidxml::xml_node<>* decl = doc.allocate_node(rapidxml::node_declaration);
	decl->append_attribute(doc.allocate_attribute("version", "1.0"));
	decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
	doc.append_node(decl);

	rapidxml::xml_node<>* entitiesNode = doc.allocate_node(rapidxml::node_element, "Entities");
	doc.append_node(entitiesNode);
	for (Entity& ent: entities) {

		rapidxml::xml_node<>* entity = doc.allocate_node(rapidxml::node_element, "Entity");
		entity->append_attribute(doc.allocate_attribute("Name", doc.allocate_string(ent.name.c_str())));
		entitiesNode->append_node(entity);
	}

	std::string xml_as_string;
	rapidxml::print(std::back_inserter(xml_as_string), doc);

	std::ofstream file_stored(editor->fileSystem.assetsPathExternal + "\\MyProject\\Database\\entity_db.xml");
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

	rapidxml::xml_node<>* meshRenderersNode = doc.allocate_node(rapidxml::node_element, "MeshRendererComponents");
	doc.append_node(meshRenderersNode);

	for (auto& it : editor->scene.meshRendererComponents) {
	
		rapidxml::xml_node<>* componentNode = doc.allocate_node(rapidxml::node_element, "MeshRendererComponent");
		componentNode->append_attribute(doc.allocate_attribute("EntID", doc.allocate_string(std::to_string(it.second.entID).c_str())));
		componentNode->append_attribute(doc.allocate_attribute("MeshName", doc.allocate_string(it.second.mesh->name.c_str())));
		componentNode->append_attribute(doc.allocate_attribute("MatName", doc.allocate_string(it.second.mat->name.c_str())));
		meshRenderersNode->append_node(componentNode);
	}

	std::string xml_as_string;
	rapidxml::print(std::back_inserter(xml_as_string), doc);

	std::ofstream file_stored(editor->fileSystem.assetsPathExternal + "\\MyProject\\Database\\meshrenderer_db.xml");
	file_stored << doc;
	file_stored.close();
	doc.clear();
}

//void Scene::saveLights() {
//
//	rapidxml::xml_document<> doc;
//	rapidxml::xml_node<>* decl = doc.allocate_node(rapidxml::node_declaration);
//	decl->append_attribute(doc.allocate_attribute("version", "1.0"));
//	decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
//	doc.append_node(decl);
//
//	rapidxml::xml_node<>* lightsNode = doc.allocate_node(rapidxml::node_element, "Lights");
//	doc.append_node(lightsNode);
//	for (std::map<int, LightComponent>::iterator it = lightComponents.begin(); it != lightComponents.end(); ++it) {
//
//		rapidxml::xml_node<>* lightNode = doc.allocate_node(rapidxml::node_element, "Light");
//
//		std::string temp_str = std::to_string(it->second.id);
//		const char* temp_val = doc.allocate_string(temp_str.c_str());
//
//		lightNode->append_attribute(doc.allocate_attribute("ID", temp_val));
//
//		std::string type = Scene::getLightType(lightComponents[it->second.id].type);
//		temp_val = doc.allocate_string(type.c_str());
//
//		lightNode->append_attribute(doc.allocate_attribute("Type", temp_val));
//
//		temp_str = std::to_string(it->second.power);
//		temp_val = doc.allocate_string(temp_str.c_str());
//
//		lightNode->append_attribute(doc.allocate_attribute("Power", temp_val));
//
//		rapidxml::xml_node<>* colorNode = doc.allocate_node(rapidxml::node_element, "Color");
//
//		temp_str = std::to_string(it->second.color.x);
//		temp_val = doc.allocate_string(temp_str.c_str());
//		colorNode->append_attribute(doc.allocate_attribute("X", temp_val));
//
//		temp_str = std::to_string(it->second.color.y);
//		temp_val = doc.allocate_string(temp_str.c_str());
//		colorNode->append_attribute(doc.allocate_attribute("Y", temp_val));
//
//		temp_str = std::to_string(it->second.color.z);
//		temp_val = doc.allocate_string(temp_str.c_str());
//		colorNode->append_attribute(doc.allocate_attribute("Z", temp_val));
//
//		lightNode->append_node(colorNode);
//
//		lightsNode->append_node(lightNode);
//	}
//
//	std::string xml_as_string;
//	rapidxml::print(std::back_inserter(xml_as_string), doc);
//
//	std::ofstream file_stored("resource/backup/light_db.xml");
//	file_stored << doc;
//	file_stored.close();
//	doc.clear();
//}

std::string Scene::getLightType(LightType type) {

	switch (type) {

	case LightType::PointLight:
		return "Point";
	case LightType::DirectionalLight:
		return "Directional";
	}
}

void Scene::saveTransformAttributes() {

	rapidxml::xml_document<> doc;
	rapidxml::xml_node<>* decl = doc.allocate_node(rapidxml::node_declaration);
	decl->append_attribute(doc.allocate_attribute("version", "1.0"));
	decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
	doc.append_node(decl);

	rapidxml::xml_node<>* transformsNode = doc.allocate_node(rapidxml::node_element, "Transforms");
	doc.append_node(transformsNode);
	for (Entity& ent: entities) {

		rapidxml::xml_node<>* transformNode = doc.allocate_node(rapidxml::node_element, "Transform");
		transformsNode->append_node(transformNode);

		rapidxml::xml_node<>* positionNode = doc.allocate_node(rapidxml::node_element, "Position");
		positionNode->append_attribute(doc.allocate_attribute("X", doc.allocate_string(std::to_string(ent.transform->position.x).c_str())));
		positionNode->append_attribute(doc.allocate_attribute("Y", doc.allocate_string(std::to_string(ent.transform->position.y).c_str())));
		positionNode->append_attribute(doc.allocate_attribute("Z", doc.allocate_string(std::to_string(ent.transform->position.z).c_str())));
		transformNode->append_node(positionNode);

		rapidxml::xml_node<>* rotationNode = doc.allocate_node(rapidxml::node_element, "Rotation");
		rotationNode->append_attribute(doc.allocate_attribute("X", doc.allocate_string(std::to_string(ent.transform->rotation.x).c_str())));
		rotationNode->append_attribute(doc.allocate_attribute("Y", doc.allocate_string(std::to_string(ent.transform->rotation.y).c_str())));
		rotationNode->append_attribute(doc.allocate_attribute("Z", doc.allocate_string(std::to_string(ent.transform->rotation.z).c_str())));
		transformNode->append_node(rotationNode);

		rapidxml::xml_node<>* scaleNode = doc.allocate_node(rapidxml::node_element, "Scale");
		scaleNode->append_attribute(doc.allocate_attribute("X", doc.allocate_string(std::to_string(ent.transform->scale.x).c_str())));
		scaleNode->append_attribute(doc.allocate_attribute("Y", doc.allocate_string(std::to_string(ent.transform->scale.y).c_str())));
		scaleNode->append_attribute(doc.allocate_attribute("Z", doc.allocate_string(std::to_string(ent.transform->scale.z).c_str())));
		transformNode->append_node(scaleNode);
	}

	std::string xml_as_string;
	rapidxml::print(std::back_inserter(xml_as_string), doc);

	std::ofstream file_stored(editor->fileSystem.assetsPathExternal + "\\MyProject\\Database\\transform_db.xml");
	file_stored << doc;
	file_stored.close();
	doc.clear();
}

void Scene::saveSceneGraph() {

	std::ofstream MyFile(editor->fileSystem.assetsPathExternal + "\\MyProject\\Database\\scenegraph_db.txt");

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