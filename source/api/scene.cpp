#include "editor.hpp"
#include "scene.hpp"

Scene::Scene() {

	name = "MyScene";	
}

void Scene::init(Editor* editor) {

	this->editor = editor;
	Scene::initSceneGraph();
}

void Scene::initSceneGraph() {

	mousepick.init();
	rootTransform = new Transform;

	if (!SaveLoadSystem::loadSceneGraph(editor)) {

		Entity entitity((const char*)"Root", rootTransform, entities);
		Scene::saveEditorProperties();
	}
	else {

		SaveLoadSystem::loadEntities(editor);
		SaveLoadSystem::loadMeshRenderers(editor);
		SaveLoadSystem::loadLights(editor);
		Scene::generateSceneGraph();
		SaveLoadSystem::loadTransforms(editor);
		SaveLoadSystem::loadPhysicsComponents(editor);
	}
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
		glUniformMatrix4fv(glGetUniformLocation(val.mat->programID, "M"), 1, GL_FALSE, &entities[val.entID].transform->model[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(val.mat->programID, "V"), 1, GL_FALSE, &editor->editorCamera.ViewMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(val.mat->programID, "P"), 1, GL_FALSE, &editor->editorCamera.ProjectionMatrix[0][0]);
		glUniform3fv(glGetUniformLocation(val.mat->programID, "camPos"), 1, &editor->editorCamera.position[0]);

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

		for (int i = 0; i < val.mat->textureUnits.size(); i++) {

			char str[16];
			sprintf(str, "texture%d\0", i);

			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, val.mat->textureUnits[i]);
			glUniform1i(glGetUniformLocation(val.mat->programID, str), i);
		}

		for (int i = 0; i < val.mat->floatUnits.size(); i++) {

			char str[16];
			sprintf(str, "float%d\0", i);
			glUniform1f(glGetUniformLocation(val.mat->programID, str), val.mat->floatUnits[i]);
		}

		glBindVertexArray(val.VAO);
		glDrawElements(GL_TRIANGLES, val.indiceSize, GL_UNSIGNED_INT, (void*)0);
		glBindVertexArray(0);
	}
}

void Scene::recompileAllMaterials() {

	for (auto& val : editor->fileSystem.materials)
		val.second.compileShaders(editor->fileSystem.getVertShaderPath(val.second.vertShaderFileAddr),
			editor->fileSystem.getFragShaderPath(val.second.fragShaderFileAddr), dirLightCount, pointLightCount);
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
	entities[toBeMoved].transform->updateSelfAndChildTransforms(3);

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

	std::vector<Entity> newEntList;

	std::vector<std::pair<int, int>> m_renderer_transform_PairList;
	std::vector<std::pair<int, int>> light_transform_PairList;
	std::vector<std::pair<int, int>> physics_transform_PairList;

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

			if (ent.physicsComponentIndex != -1) {

				std::pair<int, int> pair;
				pair.first = ent.physicsComponentIndex;
				pair.second = ent.transform->id;
				physics_transform_PairList.push_back(pair);
			}

			counter++;
		}
	}

	for (auto& iter : indices) {

		delete[] entities[iter].name;
		delete entities[iter].transform;
	}

	entities = newEntList;

	std::vector<MeshRenderer> newMeshRendererComponentList;
	std::vector<Light> newLightComponentList;
	std::vector<PhysicsComponent> newPhysicsComponentList;

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

	counter = 0;
	for (auto& pair : physics_transform_PairList) {

		newPhysicsComponentList.push_back(physicsComponents[pair.first]);
		newPhysicsComponentList[counter].entID = pair.second;
		entities[pair.second].physicsComponentIndex = counter;
		counter++;
	}
	physicsComponents = newPhysicsComponentList;
}

int Scene::duplicateEntity(int id) {

	Transform* transform = new Transform(entities[id].transform->parent, entities[id].transform, entities.size());

	char* name = new char[strlen(entities[id].name) + 5];
	strcpy(name, entities[id].name);
	strcat(name, "_cpy\0");
	Entity ent(name, transform, entities);

	Scene::cloneComponents(id, transform->id);
	cloneEntityRecursively(entities[id].transform, transform);

	transform->updateSelfAndChild();

	return transform->id;
}

void Scene::cloneEntityRecursively(Transform* base, Transform* copied) {

	for (int i = 0; i < base->children.size(); i++) {

		Transform* transform = new Transform(copied, base->children[i], entities.size());
		int len = strlen(entities[base->children[i]->id].name);
		char* name = new char[len + 1];
		strcpy(name, entities[base->children[i]->id].name);
		name[len] = '\0';
		Entity ent(name, transform, entities);

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

	if (entities[base].physicsComponentIndex != -1) {

		PhysicsComponent comp = physicsComponents[entities[base].physicsComponentIndex];
		comp.entID = entID;
		physicsComponents.push_back(comp);
		entities[entID].physicsComponentIndex = physicsComponents.size() - 1;
	}
}

Transform* Scene::newEntity(int parentID, const char* name){

	Transform* transform = new Transform(entities[parentID].transform, entities.size());
	Entity ent(name, transform, entities);

	return transform;
}

int Scene::newLight(int parentID, const char* name, LightType type) {

	Transform* entity = Scene::newEntity(parentID, name);
	entities[entity->id].addLightComponent(lightComponents, this, type);
	return entity->id;
}

void Scene::renameEntity(int id, char* newName) {

	strcpy(entities[id].name, newName);
}

void Scene::saveEditorProperties() {

	SaveLoadSystem::saveSceneGraph(editor);
	SaveLoadSystem::saveEntities(editor);
	SaveLoadSystem::saveTransforms(editor);
	SaveLoadSystem::saveMeshRenderers(editor);
	SaveLoadSystem::saveLights(editor);
	SaveLoadSystem::savePhysicsComponents(editor);
} 

std::string Scene::getLightType(LightType type) {

	switch (type) {

	case LightType::PointLight:
		return "Point";
	case LightType::DirectionalLight:
		return "Directional";
	}
}

void Scene::setEditor(Editor* editor) { this->editor = editor; }