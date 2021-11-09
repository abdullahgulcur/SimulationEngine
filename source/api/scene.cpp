#include "editor.hpp"
#include "scene.hpp"

Scene::Scene() {

	name = "MyScene";	
}

void Scene::initSceneGraph() {

	rootTransform = new Transform;

	if (!Scene::readSceneGraph()) {

		Entity entitity((const char*)"Root", rootTransform, entities);
	}
	else {

		Scene::loadEntities();
		Scene::loadMeshRenderers();
		Scene::loadLights();
		Scene::generateSceneGraph();
		Scene::loadTransformAttributes();
	}

	Scene::saveEditorProperties();
}

void Scene::generateSceneGraph() {

	entities[rootTransform->id].addTransformComponent(rootTransform);
	Scene::generateSceneGraphRecursively(rootTransform);
}

void Scene::generateSceneGraphRecursively(Transform* parent) {

	for (int i = 0; i < initialSceneGraph[parent->id].size(); i++) {

		Transform* transform = new Transform(parent, initialSceneGraph[parent->id][i]);
		entities[transform->id].addTransformComponent(transform);
		Scene::generateSceneGraphRecursively(transform);
	}
}

void Scene::start() {

}

void Scene::update() {

	for (auto const& val : meshRendererComponents)
	{
		glUseProgram(val.mat->programID);
		glUniformMatrix4fv(val.mat->mID, 1, GL_FALSE, &entities[val.entID].transform->model[0][0]);
		glUniformMatrix4fv(val.mat->vID, 1, GL_FALSE, &editor->editorCamera.ViewMatrix[0][0]);
		glUniformMatrix4fv(val.mat->pID, 1, GL_FALSE, &editor->editorCamera.ProjectionMatrix[0][0]);
		glUniform3fv(val.mat->camPosID, 1, &editor->editorCamera.position[0]);

		int dlightCounter = 0;
		int plightCounter = 0;
		for (auto const& l_val : lightComponents) {

			if (l_val.type == LightType::PointLight) {

				char lightCounterTxt[4];
				sprintf(lightCounterTxt, "%d", plightCounter);

				char* tempLPos = new char[25];
				strcpy(tempLPos, "pointLightPositions[");
				strcat(tempLPos, lightCounterTxt);
				strcat(tempLPos, "]\0");

				char* tempLCol = new char[25];
				strcpy(tempLCol, "pointLightColors[");
				strcat(tempLCol, lightCounterTxt);
				strcat(tempLCol, "]\0");

				char* tempLPow = new char[25];
				strcpy(tempLPow, "pointLightPowers[");
				strcat(tempLPow, lightCounterTxt);
				strcat(tempLPow, "]\0");

				glUniform3fv(glGetUniformLocation(val.mat->programID, tempLPos), 1, &entities[l_val.entID].transform->getWorldPosition()[0]);
				glUniform3fv(glGetUniformLocation(val.mat->programID, tempLCol), 1, &l_val.color[0]);
				glUniform1f(glGetUniformLocation(val.mat->programID, tempLPow), l_val.power);

				delete tempLPos;
				delete tempLCol;
				delete tempLPow;

				plightCounter++;
			}
			else {

				char lightCounterTxt[4];
				sprintf(lightCounterTxt, "%d", dlightCounter);

				char* tempLPos = new char[25];
				strcpy(tempLPos, "dirLightDirections[");
				strcat(tempLPos, lightCounterTxt);
				strcat(tempLPos, "]\0");

				char* tempLCol = new char[25];
				strcpy(tempLCol, "dirLightColors[");
				strcat(tempLCol, lightCounterTxt);
				strcat(tempLCol, "]\0");

				char* tempLPow = new char[25];
				strcpy(tempLPow, "dirLightPowers[");
				strcat(tempLPow, lightCounterTxt);
				strcat(tempLPow, "]\0");

				glUniform3fv(glGetUniformLocation(val.mat->programID, tempLPos), 1, &entities[l_val.entID].transform->localRotation[0]);
				glUniform3fv(glGetUniformLocation(val.mat->programID, tempLCol), 1, &l_val.color[0]);
				glUniform1f(glGetUniformLocation(val.mat->programID, tempLPow), l_val.power);

				delete tempLPos;
				delete tempLCol;
				delete tempLPow;

				dlightCounter++;
			}
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

		glBindVertexArray(val.VAO);
		glDrawElements(GL_TRIANGLES, val.indiceSize, GL_UNSIGNED_INT, (void*)0);
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

	for (rapidxml::xml_node<>* entity_node = root_node->first_node("Entity"); entity_node; entity_node = entity_node->next_sibling())
		Entity ent(entity_node->first_attribute("Name")->value(), entities);
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

		entities[count].transform->localPosition.x = atof(transform_node->first_node("Position")->first_attribute("X")->value());
		entities[count].transform->localPosition.y = atof(transform_node->first_node("Position")->first_attribute("Y")->value());
		entities[count].transform->localPosition.z = atof(transform_node->first_node("Position")->first_attribute("Z")->value());

		entities[count].transform->localRotation.x = atof(transform_node->first_node("Rotation")->first_attribute("X")->value());
		entities[count].transform->localRotation.y = atof(transform_node->first_node("Rotation")->first_attribute("Y")->value());
		entities[count].transform->localRotation.z = atof(transform_node->first_node("Rotation")->first_attribute("Z")->value());

		entities[count].transform->localScale.x = atof(transform_node->first_node("Scale")->first_attribute("X")->value());
		entities[count].transform->localScale.y = atof(transform_node->first_node("Scale")->first_attribute("Y")->value());
		entities[count].transform->localScale.z = atof(transform_node->first_node("Scale")->first_attribute("Z")->value());

		count++;
	}

	rootTransform->updateSelfAndChild();
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

		MeshRenderer m_rendererComp;
		m_rendererComp.entID = atoi(mesh_node->first_attribute("EntID")->value());

		auto meshFound = editor->fileSystem.meshVAOs.find(mesh_node->first_attribute("MeshName")->value());
		if (meshFound != editor->fileSystem.meshVAOs.end()) {
			m_rendererComp.VAO = meshFound->second;
			m_rendererComp.indiceSize = editor->fileSystem.meshes[m_rendererComp.VAO].indiceSize;
		}
		else {
			m_rendererComp.VAO = editor->fileSystem.meshVAOs["Null"];
			m_rendererComp.indiceSize = 0;
		}

		auto matFound = editor->fileSystem.materials.find(mesh_node->first_attribute("MatName")->value());
		if (matFound != editor->fileSystem.materials.end())
			m_rendererComp.mat = &matFound->second;
		else
			m_rendererComp.mat = &editor->fileSystem.materials["Default"];

		meshRendererComponents.push_back(m_rendererComp);
		entities[m_rendererComp.entID].m_rendererComponentIndex = meshRendererComponents.size() - 1;
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

		Light lightComp;
		lightComp.entID = atoi(light_node->first_attribute("EntID")->value());

		if (strcmp(light_node->first_attribute("Type")->value(), "Point") == 0){

			pointLightCount++;
			lightComp.type = LightType::PointLight;
		}
		else {

			dirLightCount++;
			lightComp.type = LightType::DirectionalLight;
		}
		
		lightComp.power = atof(light_node->first_attribute("Power")->value());

		lightComp.color.x = atof(light_node->first_node("Color")->first_attribute("X")->value());
		lightComp.color.y = atof(light_node->first_node("Color")->first_attribute("Y")->value());
		lightComp.color.z = atof(light_node->first_node("Color")->first_attribute("Z")->value());

		lightComponents.push_back(lightComp);
		entities[lightComp.entID].lightComponentIndex = lightComponents.size() - 1;
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
	entities[toBeMoved].transform->updateSelfAndChildTransforms();
	//entities[toBeMoved].transform->localScale = entities[toBeMoved].transform->scale / entities[toBeMoved].transform->parent->scale;
	//entities[toBeMoved].transform->localPosition = entities[toBeMoved].transform->position - entities[toBeMoved].transform->parent->position;
	//entities[toBeMoved].transform->localPosition *= (entities[toBeMoved].transform->scale / entities[toBeMoved].transform->parent->scale);

	

	Scene::deleteEntityFromTree(oldParent, toBeMoved);
}

void Scene::getTreeIndices(Transform* transform, std::set<int, std::greater<int>>& indices) {

	for (int i = 0; i < transform->children.size(); i++)
		getTreeIndices(transform->children[i], indices);

	indices.insert(transform->id);
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

	std::set<int, std::greater<int>> indices;
	Scene::getTreeIndices(entities[id].transform, indices);

	Transform* parent = entities[id].transform->parent;
	Scene::deleteEntityFromTree(parent, id);

	for (auto& iter : indices) {

		//delete entities[iter].name;
		//delete entities[iter].transform;
	}

	std::vector<Entity> newEntList;

	std::vector<std::pair<int, int>> m_renderer_transform_PairList;
	std::vector<std::pair<int, int>> light_transform_PairList;

	int counter = 0;
	for (Entity& ent : entities) {

		if (indices.count(ent.transform->id) == 0) {

			ent.transform->id = counter;
			newEntList.push_back(ent);

			if (ent.m_rendererComponentIndex != -1) {

				std::pair<int, int> pair;
				pair.first = ent.m_rendererComponentIndex;
				pair.second = ent.transform->id;
				m_renderer_transform_PairList.push_back(pair);
			}

			if (ent.lightComponentIndex != -1) {

				std::pair<int, int> pair;
				pair.first = ent.lightComponentIndex;
				pair.second = ent.transform->id;
				light_transform_PairList.push_back(pair);
			}
			counter++;
		}
	}

	entities = newEntList;

	std::vector<MeshRenderer> newMeshRendererComponentList;
	std::vector<Light> newLightComponentList;

	counter = 0;
	for (auto& pair : m_renderer_transform_PairList) {

		newMeshRendererComponentList.push_back(meshRendererComponents[pair.first]);
		newMeshRendererComponentList[counter].entID = pair.second;
		entities[pair.second].m_rendererComponentIndex = counter;
		counter++;
	}
	meshRendererComponents = newMeshRendererComponentList;
		

	counter = 0;
	for (auto& pair : light_transform_PairList) {

		newLightComponentList.push_back(lightComponents[pair.first]);
		newLightComponentList[counter].entID = pair.second;
		entities[pair.second].lightComponentIndex = counter;
		counter++;
	}
	lightComponents = newLightComponentList;
	Scene::recompileAllMaterials();
}

void Scene::duplicateEntity(int id) {

	Transform* transform = new Transform(entities[id].transform->parent, entities[id].transform, entities.size());

	char* name = new char[strlen(entities[id].name) + 5];
	strcpy((char*)name, entities[id].name);
	strcat((char*)name, "_cpy\0");
	Entity ent(name, transform, entities);

	Scene::cloneComponents(id, transform->id);
	cloneEntityRecursively(entities[id].transform, transform);

	transform->updateSelfAndChild();
}

void Scene::cloneEntityRecursively(Transform* base, Transform* copied) {

	for (int i = 0; i < base->children.size(); i++) {

		Transform* transform = new Transform(copied, base->children[i], entities.size());
		Entity ent(entities[base->children[i]->id].name, transform, entities);

		Scene::cloneComponents(base->children[i]->id, transform->id);
		Scene::cloneEntityRecursively(base->children[i], transform);
	}
}

void Scene::cloneComponents(int base, int entID) {

	if (entities[base].m_rendererComponentIndex != -1) {

		MeshRenderer comp = meshRendererComponents[entities[base].m_rendererComponentIndex];
		comp.entID = entID;
		meshRendererComponents.push_back(comp);
		entities[entID].m_rendererComponentIndex = meshRendererComponents.size() - 1;
	}

	if (entities[base].lightComponentIndex != -1) {

		if (lightComponents[entities[base].lightComponentIndex].type == LightType::DirectionalLight)
			dirLightCount++;
		else
			pointLightCount++;

		Scene::recompileAllMaterials();

		Light comp = lightComponents[entities[base].lightComponentIndex];
		comp.entID = entID;
		lightComponents.push_back(comp);
		entities[entID].lightComponentIndex = lightComponents.size() - 1;
	}
}

Transform* Scene::newEntity(int parentID, const char* name){

	Transform* transform = new Transform(entities[parentID].transform, entities.size());
	Entity ent(name, transform, entities);

	return transform;
}

void Scene::newLight(int parentID, const char* name, LightType type) {

	Transform* entity = Scene::newEntity(parentID, name);
	entities[entity->id].addLightComponent(lightComponents, this, type);
}

void Scene::renameEntity(int id, char* newName) {

	strcpy(entities[id].name, newName);
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
		entity->append_attribute(doc.allocate_attribute("Name", doc.allocate_string(ent.name)));
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
		componentNode->append_attribute(doc.allocate_attribute("EntID", doc.allocate_string(std::to_string(it.entID).c_str())));
		componentNode->append_attribute(doc.allocate_attribute("MeshName", doc.allocate_string(editor->fileSystem.meshNames[it.VAO].c_str())));
		componentNode->append_attribute(doc.allocate_attribute("MatName", doc.allocate_string(it.mat->name.c_str())));
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
	for (auto& it : editor->scene.lightComponents) {

		rapidxml::xml_node<>* lightNode = doc.allocate_node(rapidxml::node_element, "Light");
		lightNode->append_attribute(doc.allocate_attribute("EntID", doc.allocate_string(std::to_string(it.entID).c_str())));
		lightNode->append_attribute(doc.allocate_attribute("Type", doc.allocate_string(Scene::getLightType(lightComponents[entities[it.entID].lightComponentIndex].type).c_str())));
		lightNode->append_attribute(doc.allocate_attribute("Power", doc.allocate_string(std::to_string(it.power).c_str())));

		rapidxml::xml_node<>* colorNode = doc.allocate_node(rapidxml::node_element, "Color");
		colorNode->append_attribute(doc.allocate_attribute("X", doc.allocate_string(std::to_string(it.color.x).c_str())));
		colorNode->append_attribute(doc.allocate_attribute("Y", doc.allocate_string(std::to_string(it.color.y).c_str())));
		colorNode->append_attribute(doc.allocate_attribute("Z", doc.allocate_string(std::to_string(it.color.z).c_str())));
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
		positionNode->append_attribute(doc.allocate_attribute("X", doc.allocate_string(std::to_string(ent.transform->localPosition.x).c_str())));
		positionNode->append_attribute(doc.allocate_attribute("Y", doc.allocate_string(std::to_string(ent.transform->localPosition.y).c_str())));
		positionNode->append_attribute(doc.allocate_attribute("Z", doc.allocate_string(std::to_string(ent.transform->localPosition.z).c_str())));
		transformNode->append_node(positionNode);

		rapidxml::xml_node<>* rotationNode = doc.allocate_node(rapidxml::node_element, "Rotation");
		rotationNode->append_attribute(doc.allocate_attribute("X", doc.allocate_string(std::to_string(ent.transform->localRotation.x).c_str())));
		rotationNode->append_attribute(doc.allocate_attribute("Y", doc.allocate_string(std::to_string(ent.transform->localRotation.y).c_str())));
		rotationNode->append_attribute(doc.allocate_attribute("Z", doc.allocate_string(std::to_string(ent.transform->localRotation.z).c_str())));
		transformNode->append_node(rotationNode);

		rapidxml::xml_node<>* scaleNode = doc.allocate_node(rapidxml::node_element, "Scale");
		scaleNode->append_attribute(doc.allocate_attribute("X", doc.allocate_string(std::to_string(ent.transform->localScale.x).c_str())));
		scaleNode->append_attribute(doc.allocate_attribute("Y", doc.allocate_string(std::to_string(ent.transform->localScale.y).c_str())));
		scaleNode->append_attribute(doc.allocate_attribute("Z", doc.allocate_string(std::to_string(ent.transform->localScale.z).c_str())));
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