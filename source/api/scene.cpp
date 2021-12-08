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

	std::map<int, std::vector<int>> initialSceneGraph;
	if (!SaveLoadSystem::loadSceneGraph(editor, initialSceneGraph)) {

		Entity ent((const char*)"Root", entities);
		entities.push_back(ent);

		Scene::saveEditorProperties();
	}
	else {

		SaveLoadSystem::loadEntities(editor);
		Scene::generateSceneGraph(initialSceneGraph);
		Scene::loadLights();
		entities[0].transform->updateSelfAndChild();
	}
}

void Scene::generateSceneGraph(std::map<int, std::vector<int>>& initialSceneGraph) {

	Scene::generateSceneGraphRecursively(entities[0].transform, initialSceneGraph);
}

void Scene::generateSceneGraphRecursively(Transform* parent, std::map<int, std::vector<int>>& initialSceneGraph) {

	for (int i = 0; i < initialSceneGraph[parent->id].size(); i++) {

		Transform* transform = entities[initialSceneGraph[parent->id][i]].transform;
		transform->id = initialSceneGraph[parent->id][i];
		transform->parent = parent;
		(parent->children).push_back(transform);
		Scene::generateSceneGraphRecursively(transform, initialSceneGraph);
	}
}

void Scene::loadLights() {

	for (auto& it : entities) {

		Light* lightComp = it.getComponent<Light>();

		if (lightComp != NULL) {

			if (lightComp->lightType == LightType::PointLight)
				editor->scene.pointLightTransforms.push_back(it.transform);
			else
				editor->scene.dirLightTransforms.push_back(it.transform);
		}
	}

	editor->scene.recompileAllMaterials();
}

void Scene::start() {

}

void Scene::update(float dt) {

	for (auto& val : editor->scene.entities)
	{
		MeshRenderer* meshRendererComp = val.getComponent<MeshRenderer>();

		if (meshRendererComp == nullptr)
			continue;

		glUseProgram(meshRendererComp->mat->programID);
		glUniformMatrix4fv(glGetUniformLocation(meshRendererComp->mat->programID, "M"), 1, GL_FALSE, &val.transform->model[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(meshRendererComp->mat->programID, "V"), 1, GL_FALSE, &editor->editorCamera.ViewMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(meshRendererComp->mat->programID, "P"), 1, GL_FALSE, &editor->editorCamera.ProjectionMatrix[0][0]);
		glUniform3fv(glGetUniformLocation(meshRendererComp->mat->programID, "camPos"), 1, &editor->editorCamera.position[0]);

		int dlightCounter = 0;
		int plightCounter = 0;

		for (auto const& pl_val : pointLightTransforms) {
		
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

			glUniform3fv(glGetUniformLocation(meshRendererComp->mat->programID, tempLPos), 1, &pl_val->globalPosition[0]);
			glUniform3fv(glGetUniformLocation(meshRendererComp->mat->programID, tempLCol), 1, &entities[pl_val->id].getComponent<Light>()->color[0]);
			glUniform1f(glGetUniformLocation(meshRendererComp->mat->programID, tempLPow), entities[pl_val->id].getComponent<Light>()->power);

			delete tempLPos;
			delete tempLCol;
			delete tempLPow;

			plightCounter++;
		}

		for (auto const& dl_val : dirLightTransforms) {

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

			glm::vec3 direction(-cos(dl_val->localRotation.x / 180.f) * sin(dl_val->localRotation.y / 180.f), -sin(dl_val->localRotation.x / 180.f),
				-cos(dl_val->localRotation.x / 180.f) * cos(dl_val->localRotation.y / 180.f));
			glUniform3fv(glGetUniformLocation(meshRendererComp->mat->programID, tempLPos), 1, &direction[0]);
			glUniform3fv(glGetUniformLocation(meshRendererComp->mat->programID, tempLCol), 1, &entities[dl_val->id].getComponent<Light>()->color[0]);
			glUniform1f(glGetUniformLocation(meshRendererComp->mat->programID, tempLPow), entities[dl_val->id].getComponent<Light>()->power);

			delete tempLPos;
			delete tempLCol;
			delete tempLPow;

			dlightCounter++;
		}

		for (int i = 0; i < meshRendererComp->mat->textureUnits.size(); i++) {

			char str[16];
			sprintf(str, "texture%d\0", i);

			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, meshRendererComp->mat->textureUnits[i]);
			glUniform1i(glGetUniformLocation(meshRendererComp->mat->programID, str), i);
		}

		for (int i = 0; i < meshRendererComp->mat->floatUnits.size(); i++) {

			char str[16];
			sprintf(str, "float%d\0", i);
			glUniform1f(glGetUniformLocation(meshRendererComp->mat->programID, str), meshRendererComp->mat->floatUnits[i]);
		}

		glBindVertexArray(meshRendererComp->mesh->VAO);
		glDrawElements(GL_TRIANGLES, meshRendererComp->mesh->indiceSize, GL_UNSIGNED_INT, (void*)0);
		glBindVertexArray(0);
	}

	//for (auto& it : rigidbodyComponents) {

	//	PxMat44 matrix4(it.body->getGlobalPose());

	//	//it.body->set

	//	entities[it.entID].transform->model[0][0] = matrix4[0][0];
	//	entities[it.entID].transform->model[0][1] = matrix4[0][1];
	//	entities[it.entID].transform->model[0][2] = matrix4[0][2];
	//	entities[it.entID].transform->model[0][3] = matrix4[0][3];

	//	entities[it.entID].transform->model[1][0] = matrix4[1][0];
	//	entities[it.entID].transform->model[1][1] = matrix4[1][1];
	//	entities[it.entID].transform->model[1][2] = matrix4[1][2];
	//	entities[it.entID].transform->model[1][3] = matrix4[1][3];

	//	entities[it.entID].transform->model[2][0] = matrix4[2][0];
	//	entities[it.entID].transform->model[2][1] = matrix4[2][1];
	//	entities[it.entID].transform->model[2][2] = matrix4[2][2];
	//	entities[it.entID].transform->model[2][3] = matrix4[2][3];

	//	entities[it.entID].transform->model[3][0] = matrix4[3][0];
	//	entities[it.entID].transform->model[3][1] = matrix4[3][1];
	//	entities[it.entID].transform->model[3][2] = matrix4[3][2];
	//	entities[it.entID].transform->model[3][3] = matrix4[3][3];

	//	//entities[it.entID].transform->model =

	//	//entities[it.entID].transform->model = it.body->getGlobalPose();
	//	// 
	//	//if (it.useGravity) {

	//	//entities[it.entID].transform->localPosition.x = it.body->getGlobalPose().p.x;
	//	//entities[it.entID].transform->localPosition.y = it.body->getGlobalPose().p.y;
	//	//entities[it.entID].transform->localPosition.z = it.body->getGlobalPose().p.z;

	//	//entities[it.entID].transform->updateSelfAndChild();
	//	//}

	//	entities[it.entID].transform->updateSelfAndChildTransforms();

	//}

}

void Scene::recompileAllMaterials() {

	for (auto& val : editor->fileSystem.materials)
		val.second.compileShaders(editor->fileSystem.getVertShaderPath(val.second.vertShaderFileAddr),
			editor->fileSystem.getFragShaderPath(val.second.fragShaderFileAddr), dirLightTransforms.size(), pointLightTransforms.size());
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

	Scene::deleteEntityFromTree(oldParent, toBeMoved);
}

void Scene::getTreeIndices(Transform* transform, std::unordered_set<int>& indices) {

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

	std::unordered_set<int> indices;
	Scene::getTreeIndices(entities[id].transform, indices);

	Transform* parent = entities[id].transform->parent;
	Scene::deleteEntityFromTree(parent, id);

	entities.erase(
		std::remove_if(entities.begin(), entities.end(), [&](Entity ent) {

			bool b = indices.find(ent.transform->id) != indices.end();
			if (b)
				ent.destroy(this);

			return b; 
		}), entities.end());

	int i = 0;
	for (auto& it : entities)
		it.transform->id = i++;
}

int Scene::duplicateEntity(int id) {

	Entity ent(&entities[id], entities[id].transform->parent, *this);
	entities.push_back(ent);

	cloneEntityRecursively(entities[id].transform, editor->scene.entities.back().transform);
	editor->scene.entities[ent.transform->id].transform->updateSelfAndChild();

	return ent.transform->id;
}

void Scene::cloneEntityRecursively(Transform* base, Transform* copied) {

	for (int i = 0; i < base->children.size(); i++) {

		Entity ent(&entities[base->children[i]->id], copied, *this);
		entities.push_back(ent);

		Scene::cloneEntityRecursively(base->children[i], editor->scene.entities.back().transform);
	}
}

Transform* Scene::newEntity(int parentID, const char* name){

	Entity ent(name, &entities[parentID], *this);
	entities.push_back(ent);

	return ent.transform;
}

int Scene::newLight(int parentID, const char* name, LightType type) {

	Transform* transform = Scene::newEntity(parentID, name);

	Light* lightComp = entities[transform->id].addComponent<Light>();
	lightComp->lightType = type;

	Transform* lightTransform = entities[transform->id].transform;

	if (type == LightType::PointLight)
		editor->scene.pointLightTransforms.push_back(lightTransform);
	else
		editor->scene.dirLightTransforms.push_back(lightTransform);

	recompileAllMaterials();

	return transform->id;
}

void Scene::renameEntity(int id, char* newName) {

	strcpy(entities[id].name, newName);
}

void Scene::saveEditorProperties() {

	SaveLoadSystem::saveSceneGraph(editor);
	SaveLoadSystem::saveEntities(editor);
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