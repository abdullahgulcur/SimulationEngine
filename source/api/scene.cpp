#include "editor.hpp"
#include "scene.hpp"

Scene::Scene() {

	name = "MyScene";	
}

void Scene::init(Editor* editor) {

	this->editor = editor;
	mousepick.init();
	Scene::initSceneGraph();
}

void Scene::initSceneGraph() {

	SaveLoadSystem::loadEntities(editor);
	Scene::loadLights();
}

void Scene::loadLights() {

	for (auto& ent : entities) {

		if (Light* lightComp = ent->getComponent<Light>()) {

			if (lightComp->lightType == LightType::PointLight)
				pointLightTransforms.push_back(ent->transform);
			else
				dirLightTransforms.push_back(ent->transform);
		}
	}

	Scene::recompileAllMaterials();
}

void Scene::start() {

}

void Scene::update(float dt) {

	if (editor->gameStarted)
		Scene::simulateInGame(dt);
	else
		Scene::simulateInEditor(dt);
}

void Scene::simulateInEditor(float dt) {

	for (auto& ent : entities)
	{
		if (MeshRenderer* meshRendererComp = ent->getComponent<MeshRenderer>()) {

			glUseProgram(meshRendererComp->mat->programID);
			glUniformMatrix4fv(glGetUniformLocation(meshRendererComp->mat->programID, "M"), 1, GL_FALSE, &ent->transform->model[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(meshRendererComp->mat->programID, "V"), 1, GL_FALSE, &editor->editorCamera.ViewMatrix[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(meshRendererComp->mat->programID, "P"), 1, GL_FALSE, &editor->editorCamera.ProjectionMatrix[0][0]);
			glUniform3fv(glGetUniformLocation(meshRendererComp->mat->programID, "camPos"), 1, &editor->editorCamera.position[0]);

			int dlightCounter = 0;
			int plightCounter = 0;

			for (auto const& transform : pointLightTransforms) {

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

				glUniform3fv(glGetUniformLocation(meshRendererComp->mat->programID, tempLPos), 1, &transform->globalPosition[0]);
				glUniform3fv(glGetUniformLocation(meshRendererComp->mat->programID, tempLCol), 1, &transform->entity->getComponent<Light>()->color[0]);
				glUniform1f(glGetUniformLocation(meshRendererComp->mat->programID, tempLPow), transform->entity->getComponent<Light>()->power);

				delete tempLPos;
				delete tempLCol;
				delete tempLPow;

				plightCounter++;
			}

			for (auto const& transform : dirLightTransforms) {

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

				glm::vec3 direction(-cos(transform->localRotation.x / 180.f) * sin(transform->localRotation.y / 180.f), -sin(transform->localRotation.x / 180.f),
					-cos(transform->localRotation.x / 180.f) * cos(transform->localRotation.y / 180.f));
				glUniform3fv(glGetUniformLocation(meshRendererComp->mat->programID, tempLPos), 1, &direction[0]);
				glUniform3fv(glGetUniformLocation(meshRendererComp->mat->programID, tempLCol), 1, &transform->entity->getComponent<Light>()->color[0]);
				glUniform1f(glGetUniformLocation(meshRendererComp->mat->programID, tempLPow), transform->entity->getComponent<Light>()->power);

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

		std::vector<BoxCollider*> boxColliderCompList = ent->getComponents<BoxCollider>();
		for (auto& comp : boxColliderCompList) {

			glm::mat4 model = ent->transform->model;
			model = glm::translate(model, comp->center);
			model = glm::scale(model, comp->size);
			glm::mat4 mvp = editor->editorCamera.ProjectionMatrix * editor->editorCamera.ViewMatrix * model;

			editor->bcr->drawCollider(mvp);
		}

		std::vector<SphereCollider*> sphereColliderCompList = ent->getComponents<SphereCollider>();
		for (auto& comp : sphereColliderCompList) {

			glm::mat4 model = ent->transform->model;
			model = glm::translate(model, comp->center);
			model = glm::scale(model, glm::vec3(comp->radius * 2 * 1.01f));
			glm::mat4 mvp = editor->editorCamera.ProjectionMatrix * editor->editorCamera.ViewMatrix * model;
			editor->scr->drawCollider(mvp);
		}

		std::vector<CapsuleCollider*> capsuleColliderCompList = ent->getComponents<CapsuleCollider>();
		for (auto& comp : capsuleColliderCompList) {

			glm::mat4 localModel(1.f);
			localModel = glm::scale(localModel, glm::vec3(comp->radius * 2 * 1.01f));

			const glm::mat4 transformX = glm::rotate(glm::mat4(1.0f),
				ent->transform->globalRotation.x,
				glm::vec3(1.0f, 0.0f, 0.0f));
			const glm::mat4 transformY = glm::rotate(glm::mat4(1.0f),
				ent->transform->globalRotation.y,
				glm::vec3(0.0f, 1.0f, 0.0f));
			const glm::mat4 transformZ = glm::rotate(glm::mat4(1.0f),
				ent->transform->globalRotation.z,
				glm::vec3(0.0f, 0.0f, 1.0f));

			const glm::mat4 roationMatrix = transformZ * transformY * transformX;

			glm::mat4 objectModel(1.f);
			objectModel = glm::translate(objectModel, ent->transform->globalPosition) * roationMatrix;
			objectModel = glm::translate(objectModel, comp->center + glm::vec3(comp->axis) * (comp->height / 2));
			glm::mat4 model = objectModel * localModel;
			glm::mat4 mvp = editor->editorCamera.ProjectionMatrix * editor->editorCamera.ViewMatrix * model;
			editor->scr->drawCollider(mvp);
			objectModel = glm::translate(objectModel, glm::vec3(comp->axis) * (-comp->height));
			model = objectModel * localModel;
			mvp = editor->editorCamera.ProjectionMatrix * editor->editorCamera.ViewMatrix * model;
			editor->scr->drawCollider(mvp);
		}

		//std::vector<MeshCollider*> meshColliderCompList = ent->getComponents<MeshCollider>();
		//for (auto& comp : meshColliderCompList) {

		//	glm::mat4 mvp = editor->editorCamera.ProjectionMatrix * editor->editorCamera.ViewMatrix * ent->transform->model;
		//}

	}
}

void Scene::simulateInGame(float dt) {

	for (auto& ent : entities)
	{
		if (MeshRenderer* meshRendererComp = ent->getComponent<MeshRenderer>()) {

			glUseProgram(meshRendererComp->mat->programID);
			glUniformMatrix4fv(glGetUniformLocation(meshRendererComp->mat->programID, "M"), 1, GL_FALSE, &ent->transform->model[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(meshRendererComp->mat->programID, "V"), 1, GL_FALSE, &editor->editorCamera.ViewMatrix[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(meshRendererComp->mat->programID, "P"), 1, GL_FALSE, &editor->editorCamera.ProjectionMatrix[0][0]);
			glUniform3fv(glGetUniformLocation(meshRendererComp->mat->programID, "camPos"), 1, &editor->editorCamera.position[0]);

			int dlightCounter = 0;
			int plightCounter = 0;

			for (auto const& transform : pointLightTransforms) {

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

				glUniform3fv(glGetUniformLocation(meshRendererComp->mat->programID, tempLPos), 1, &transform->globalPosition[0]);
				glUniform3fv(glGetUniformLocation(meshRendererComp->mat->programID, tempLCol), 1, &transform->entity->getComponent<Light>()->color[0]);
				glUniform1f(glGetUniformLocation(meshRendererComp->mat->programID, tempLPow), transform->entity->getComponent<Light>()->power);

				delete tempLPos;
				delete tempLCol;
				delete tempLPow;

				plightCounter++;
			}

			for (auto const& transform : dirLightTransforms) {

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

				glm::vec3 direction(-cos(transform->localRotation.x / 180.f) * sin(transform->localRotation.y / 180.f), -sin(transform->localRotation.x / 180.f),
					-cos(transform->localRotation.x / 180.f) * cos(transform->localRotation.y / 180.f));
				glUniform3fv(glGetUniformLocation(meshRendererComp->mat->programID, tempLPos), 1, &direction[0]);
				glUniform3fv(glGetUniformLocation(meshRendererComp->mat->programID, tempLCol), 1, &transform->entity->getComponent<Light>()->color[0]);
				glUniform1f(glGetUniformLocation(meshRendererComp->mat->programID, tempLPow), transform->entity->getComponent<Light>()->power);

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

		std::vector<BoxCollider*> boxColliderCompList = ent->getComponents<BoxCollider>();
		for (auto& comp : boxColliderCompList) {

			glm::mat4 model = ent->transform->model;
			model = glm::translate(model, comp->center);
			model = glm::scale(model, comp->size);
			glm::mat4 mvp = editor->editorCamera.ProjectionMatrix * editor->editorCamera.ViewMatrix * model;

			editor->bcr->drawCollider(mvp);
		}

		std::vector<SphereCollider*> sphereColliderCompList = ent->getComponents<SphereCollider>();
		for (auto& comp : sphereColliderCompList) {

			glm::mat4 model = ent->transform->model;
			model = glm::translate(model, comp->center);
			model = glm::scale(model, glm::vec3(comp->radius * 2 * 1.01f));
			glm::mat4 mvp = editor->editorCamera.ProjectionMatrix * editor->editorCamera.ViewMatrix * model;
			editor->scr->drawCollider(mvp);
		}

		std::vector<CapsuleCollider*> capsuleColliderCompList = ent->getComponents<CapsuleCollider>();
		for (auto& comp : capsuleColliderCompList) {

			glm::mat4 localModel(1.f);
			localModel = glm::scale(localModel, glm::vec3(comp->radius * 2 * 1.01f));

			const glm::mat4 transformX = glm::rotate(glm::mat4(1.0f),
				ent->transform->globalRotation.x,
				glm::vec3(1.0f, 0.0f, 0.0f));
			const glm::mat4 transformY = glm::rotate(glm::mat4(1.0f),
				ent->transform->globalRotation.y,
				glm::vec3(0.0f, 1.0f, 0.0f));
			const glm::mat4 transformZ = glm::rotate(glm::mat4(1.0f),
				ent->transform->globalRotation.z,
				glm::vec3(0.0f, 0.0f, 1.0f));

			const glm::mat4 roationMatrix = transformZ * transformY * transformX;

			glm::mat4 objectModel(1.f);
			objectModel = glm::translate(objectModel, ent->transform->globalPosition) * roationMatrix;
			objectModel = glm::translate(objectModel, comp->center + glm::vec3(comp->axis) * (comp->height / 2));
			glm::mat4 model = objectModel * localModel;
			glm::mat4 mvp = editor->editorCamera.ProjectionMatrix * editor->editorCamera.ViewMatrix * model;
			editor->scr->drawCollider(mvp);
			objectModel = glm::translate(objectModel, glm::vec3(comp->axis) * (-comp->height));
			model = objectModel * localModel;
			mvp = editor->editorCamera.ProjectionMatrix * editor->editorCamera.ViewMatrix * model;
			editor->scr->drawCollider(mvp);
		}

		std::vector<MeshCollider*> meshColliderCompList = ent->getComponents<MeshCollider>();
		for (auto& comp : meshColliderCompList) {

			glm::mat4 mvp = editor->editorCamera.ProjectionMatrix * editor->editorCamera.ViewMatrix * ent->transform->model;
		}

		if (Rigidbody* rigidbodyComp = ent->getComponent<Rigidbody>()) {

			PxMat44 matrix4(rigidbodyComp->body->getGlobalPose());

			float scaleX = length(ent->transform->model[0]);
			float scaleY = length(ent->transform->model[1]);
			float scaleZ = length(ent->transform->model[2]);
			glm::vec3 scale(scaleX, scaleY, scaleZ);

			ent->transform->model[0][0] = matrix4[0][0];
			ent->transform->model[0][1] = matrix4[0][1];
			ent->transform->model[0][2] = matrix4[0][2];
			ent->transform->model[0][3] = matrix4[0][3];

			ent->transform->model[1][0] = matrix4[1][0];
			ent->transform->model[1][1] = matrix4[1][1];
			ent->transform->model[1][2] = matrix4[1][2];
			ent->transform->model[1][3] = matrix4[1][3];

			ent->transform->model[2][0] = matrix4[2][0];
			ent->transform->model[2][1] = matrix4[2][1];
			ent->transform->model[2][2] = matrix4[2][2];
			ent->transform->model[2][3] = matrix4[2][3];

			ent->transform->model[3][0] = matrix4[3][0];
			ent->transform->model[3][1] = matrix4[3][1];
			ent->transform->model[3][2] = matrix4[3][2];
			ent->transform->model[3][3] = matrix4[3][3];

			ent->transform->model = glm::scale(ent->transform->model, scale);

			ent->transform->updateSelfAndChildTransforms();
		}
	}

	editor->physics.gScene->simulate(dt);
	editor->physics.gScene->fetchResults(true);
}

void Scene::deleteScene() {

	for (auto& ent : entities)
		ent->destroy(this);

	entities.clear();
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

void Scene::moveEntity(Entity* toBeMoved, Entity* moveTo) {

	if (Scene::subEntityCheck(moveTo->transform, toBeMoved->transform))
		return;

	Transform* oldParent = toBeMoved->transform->parent;
	toBeMoved->transform->parent = moveTo->transform;
	(moveTo->transform->children).push_back(toBeMoved->transform);
	toBeMoved->transform->updateSelfAndChildTransforms();

	Scene::deleteEntityFromTree(oldParent, toBeMoved);
}

void Scene::getAllEntities(Transform* transform, std::unordered_set<Entity*>& ents) {

	for (int i = 0; i < transform->children.size(); i++)
		getAllEntities(transform->children[i], ents);

	ents.insert(transform->entity);
}

void Scene::deleteEntityFromTree(Transform* parent, Entity* ent) {

	for (int i = 0; i < parent->children.size(); i++) {

		if (parent->children[i] == ent->transform) {

			parent->children.erase(parent->children.begin() + i);
			break;
		}
	}
}

void Scene::deleteEntityCompletely(Entity* ent) {

	std::unordered_set<Entity*> ents;
	Scene::getAllEntities(ent->transform, ents);

	Transform* parent = ent->transform->parent;
	Scene::deleteEntityFromTree(parent, ent);

	entities.erase(
		std::remove_if(entities.begin(), entities.end(), [&](Entity* ent) {

			bool b = ents.find(ent) != ents.end();
			if (b)
				ent->destroy(this);

			return b; 
		}), entities.end());
}

Entity* Scene::duplicateEntity(Entity* ent) {

	Entity* entity = new Entity(ent, ent->transform->parent, this);
	cloneEntityRecursively(ent->transform, entity->transform);
	entity->transform->updateSelfAndChild();

	return entity;
}

void Scene::cloneEntityRecursively(Transform* base, Transform* copied) {

	for (int i = 0; i < base->children.size(); i++) {

		Entity* ent = new Entity(base->children[i]->entity, copied, this);
		Scene::cloneEntityRecursively(base->children[i], ent->transform);
	}
}

Transform* Scene::newEntity(Entity* parent, const char* name){

	Entity* ent = new Entity(name, parent->transform, this);
	return ent->transform;
}

int Scene::getEntityIndex(Entity* ent) {

	auto it = std::find(entities.begin(), entities.end(), ent);

	if (it != entities.end())
		return it - entities.begin();

	return -1;
}

Entity* Scene::newLight(Entity* parent, const char* name, LightType type) {

	Transform* transform = Scene::newEntity(parent, name);

	Light* lightComp = transform->entity->addComponent<Light>();
	lightComp->lightType = type;

	if (type == LightType::PointLight)
		pointLightTransforms.push_back(transform);
	else
		dirLightTransforms.push_back(transform);

	recompileAllMaterials();

	return transform->entity;
}

void Scene::renameEntity(Entity* ent, char* newName) {

	strcpy(ent->name, newName);
}

void Scene::saveEditorProperties() {

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