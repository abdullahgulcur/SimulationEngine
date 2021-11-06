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
		Scene::loadLights();

		

		// compile shaders again...


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

}

void Scene::update() {

	for (auto const& [key, val] : meshRendererComponents)
	{
		glUseProgram(val.mat->programID);

		glm::vec3 camPos = editor->editorCamera.position;
		glm::mat4 projection = editor->editorCamera.ProjectionMatrix;
		glm::mat4 view = editor->editorCamera.ViewMatrix;
		glm::mat4 model = glm::mat4(1.0);
		model = glm::translate(model, entities[val.entID].transform->position);
		model = glm::scale(model, entities[val.entID].transform->scale);

		glUniformMatrix4fv(val.mat->mID, 1, GL_FALSE, &model[0][0]);
		glUniformMatrix4fv(val.mat->vID, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(val.mat->pID, 1, GL_FALSE, &projection[0][0]);
		glUniform3fv(val.mat->camPosID, 1, &camPos[0]);

		std::vector<glm::vec3> p_lightSourceColors;
		std::vector<glm::vec3> p_lightSourcePositions;
		std::vector<float> p_lightSourcePowers;

		std::vector<glm::vec3> d_lightSourceColors;
		std::vector<glm::vec3> d_lightSourceDirections;
		std::vector<float> d_lightSourcePowers;

		for (auto const& [l_key, l_val] : lightComponents) {

			if (l_val.type == LightType::PointLight) {

				p_lightSourceColors.push_back(l_val.color);
				p_lightSourcePositions.push_back(entities[l_val.entID].transform->position);
				p_lightSourcePowers.push_back(l_val.power);
			}
			else {

				d_lightSourceColors.push_back(l_val.color);
				d_lightSourceDirections.push_back(entities[l_val.entID].transform->rotation);
				d_lightSourcePowers.push_back(l_val.power);
			}
		}

		for (int i = 0; i < p_lightSourceColors.size(); i++) {

			std::string tempLPos = "pointLightPositions[" + std::to_string(i);
			tempLPos += "]";

			std::string tempLCol = "pointLightColors[" + std::to_string(i);
			tempLCol += "]";

			std::string tempLPow = "pointLightPowers[" + std::to_string(i);
			tempLPow += "]";

			glUniform3fv(glGetUniformLocation(val.mat->programID, tempLPos.c_str()), 1, &p_lightSourcePositions[i][0]);
			glUniform3fv(glGetUniformLocation(val.mat->programID, tempLCol.c_str()), 1, &p_lightSourceColors[i][0]);
			glUniform1f(glGetUniformLocation(val.mat->programID, tempLPow.c_str()), p_lightSourcePowers[i]);
		}

		for (int i = 0; i < d_lightSourceColors.size(); i++) {

			std::string tempLPos = "dirLightDirections[" + std::to_string(i);
			tempLPos += "]";

			std::string tempLCol = "dirLightColors[" + std::to_string(i);
			tempLCol += "]";

			std::string tempLPow = "dirLightPowers[" + std::to_string(i);
			tempLPow += "]";

			glUniform3fv(glGetUniformLocation(val.mat->programID, tempLPos.c_str()), 1, &d_lightSourceDirections[i][0]);
			glUniform3fv(glGetUniformLocation(val.mat->programID, tempLCol.c_str()), 1, &d_lightSourceColors[i][0]);
			glUniform1f(glGetUniformLocation(val.mat->programID, tempLPow.c_str()), d_lightSourcePowers[i]);
		}

		glUniform1f(val.mat->metallicAmountID, val.mat->metallicAmount);
		glUniform1f(val.mat->roughnessAmountID, val.mat->roughnessAmount);
		glUniform1f(val.mat->aoAmountID, val.mat->aoAmount);

		if (val.mat->useAlbedo) {

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, val.mat->albedoTexture);
			glUniform1i(val.mat->albedoTextureID, 0);
		}
		else {

			glUniform3fv(val.mat->albedoColorID, 1, &val.mat->albedoColor[0]);
		}

		if (val.mat->useNormal) {

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, val.mat->normalTexture);
			glUniform1i(val.mat->normalTextureID, 1);
			glUniform1f(val.mat->normalAmountID, val.mat->normalAmount);
		}

		if (val.mat->useMetallic) {

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, val.mat->metallicTexture);
			glUniform1i(val.mat->metallicTextureID, 2);
		}

		if (val.mat->useRoughness) {

			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, val.mat->roughnessTexture);
			glUniform1i(val.mat->roughnessTextureID, 3);
		}

		if (val.mat->useAO) {

			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, val.mat->aoTexture);
			glUniform1i(val.mat->aoTextureID, 4);
		}

		glBindVertexArray(val.mesh->VAO);
		glDrawElements(GL_TRIANGLES, val.mesh->indices.size(), GL_UNSIGNED_INT, (void*)0);
		glBindVertexArray(0);
	}
}

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

void Scene::loadLights() {

	std::ifstream file(editor->fileSystem.assetsPathExternal + "\\MyProject\\Database\\light_db.xml");

	if (file.fail())
		return;

	rapidxml::xml_document<> doc;
	rapidxml::xml_node<>* root_node = NULL;

	std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	buffer.push_back('\0');

	doc.parse<0>(&buffer[0]);

	root_node = doc.first_node("Lights");

	for (rapidxml::xml_node<>* light_node = root_node->first_node("Light"); light_node; light_node = light_node->next_sibling()) {

		Light comp;
		comp.entID = atoi(light_node->first_attribute("EntID")->value());

		if (strcmp(light_node->first_attribute("Type")->value(), "Point") == 0){

			pointLightCount++;
			comp.type = LightType::PointLight;
		}
		else {

			dirLightCount++;
			comp.type = LightType::DirectionalLight;
		}
		
		comp.power = atof(light_node->first_attribute("Power")->value());

		comp.color.x = atof(light_node->first_node("Color")->first_attribute("X")->value());
		comp.color.y = atof(light_node->first_node("Color")->first_attribute("Y")->value());
		comp.color.z = atof(light_node->first_node("Color")->first_attribute("Z")->value());

		lightComponents[comp.entID] = comp;
		entities[comp.entID].components.push_back(ComponentType::Light);
	}

	Scene::recompileAllMaterials();
}

void Scene::recompileAllMaterials() {

	for (auto& val : editor->fileSystem.materials) {

		val.second.pointLightCount = pointLightCount;
		val.second.dirLightCount = dirLightCount;
		val.second.compileShaders();
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

		for (int j = 0; j < entities[indices[i]].components.size(); j++) {

			switch (entities[indices[i]].components[j]) {

			case ComponentType::Light: {

				if (lightComponents[indices[i]].type == LightType::DirectionalLight)
					dirLightCount--;
				else
					pointLightCount--;

				lightComponents.erase(indices[i]); // remove component kullan oc, destreucot???
				Scene::recompileAllMaterials();
				break;
			}
			case ComponentType::MeshRenderer: {

				meshRendererComponents.erase(indices[i]); // remove component kullan oc
				break;
			}
			}
		}

		delete entities[indices[i]].transform;
		entities.erase(entities.begin() + indices[i]);
	}

	for (int i = indices[indices.size() - 1]; i < entities.size(); i++)
		entities[i].transform->id = i;
}

void Scene::duplicateEntity(int id) {

	Transform* transform = new Transform;
	transform->id = entities.size();
	transform->parent = entities[id].transform->parent;
	(transform->parent->children).push_back(transform);

	Entity ent;
	ent.name = entities[id].name + "_cpy";
	ent.transform = transform;
	entities.push_back(ent);

	Scene::cloneComponents(id, transform->id);

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

		Scene::cloneComponents(base->children[i]->id, transform->id);

		Scene::cloneEntityRecursively(base->children[i], transform);
	}
}

void Scene::cloneComponents(int base, int entID) {

	for (int i = 0; i < entities[base].components.size(); i++) {

		switch (entities[base].components[i]) {

		case ComponentType::Light: {

			if (lightComponents[base].type == LightType::DirectionalLight)
				dirLightCount++;
			else
				pointLightCount++;

			Scene::recompileAllMaterials();

			Light comp = lightComponents[base];
			comp.entID = entID;
			lightComponents[comp.entID] = comp;
			entities[entID].components.push_back(ComponentType::Light);
			break;
		}
		case ComponentType::MeshRenderer: {

			MeshRenderer comp = meshRendererComponents[base];
			comp.entID = entID;
			meshRendererComponents[comp.entID] = comp;
			entities[entID].components.push_back(ComponentType::MeshRenderer);
			break;
		}
		}
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

void Scene::newPointLight(int parentID, const char* name) {

	pointLightCount++;

	Transform* entity = Scene::newEntity(parentID, name);
	entities[entity->id].addLightComponent(lightComponents, this);
	lightComponents[entity->id].type = LightType::PointLight;
}

void Scene::newDirectionalLight(int parentID, const char* name) {

	dirLightCount++;

	Transform* entity = Scene::newEntity(parentID, name);
	entities[entity->id].addLightComponent(lightComponents, this);
	lightComponents[entity->id].type = LightType::DirectionalLight;
}

void Scene::renameEntity(int id, const char* newName) {

	entities[id].name = newName;
}

void Scene::saveEditorProperties() {

	Scene::saveSceneGraph();
	Scene::saveEntities();
	Scene::saveTransformAttributes();
	Scene::saveMeshRenderers();
	Scene::saveLights();
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

void Scene::saveLights() {

	rapidxml::xml_document<> doc;
	rapidxml::xml_node<>* decl = doc.allocate_node(rapidxml::node_declaration);
	decl->append_attribute(doc.allocate_attribute("version", "1.0"));
	decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
	doc.append_node(decl);

	rapidxml::xml_node<>* lightsNode = doc.allocate_node(rapidxml::node_element, "Lights");
	doc.append_node(lightsNode);
	for (std::unordered_map<unsigned int, Light>::iterator it = lightComponents.begin(); it != lightComponents.end(); ++it) {

		rapidxml::xml_node<>* lightNode = doc.allocate_node(rapidxml::node_element, "Light");
		lightNode->append_attribute(doc.allocate_attribute("EntID", doc.allocate_string(std::to_string(it->second.entID).c_str())));
		lightNode->append_attribute(doc.allocate_attribute("Type", doc.allocate_string(Scene::getLightType(lightComponents[it->second.entID].type).c_str())));
		lightNode->append_attribute(doc.allocate_attribute("Power", doc.allocate_string(std::to_string(it->second.power).c_str())));

		rapidxml::xml_node<>* colorNode = doc.allocate_node(rapidxml::node_element, "Color");
		colorNode->append_attribute(doc.allocate_attribute("X", doc.allocate_string(std::to_string(it->second.color.x).c_str())));
		colorNode->append_attribute(doc.allocate_attribute("Y", doc.allocate_string(std::to_string(it->second.color.y).c_str())));
		colorNode->append_attribute(doc.allocate_attribute("Z", doc.allocate_string(std::to_string(it->second.color.z).c_str())));
		lightNode->append_node(colorNode);

		lightsNode->append_node(lightNode);
	}

	std::string xml_as_string;
	rapidxml::print(std::back_inserter(xml_as_string), doc);

	std::ofstream file_stored(editor->fileSystem.assetsPathExternal + "\\MyProject\\Database\\light_db.xml");
	file_stored << doc;
	file_stored.close();
	doc.clear();
}

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