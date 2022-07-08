#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "rapidxml_print.hpp"
#include "rapidxml.hpp"

#include "entity.hpp"
#include "transform.hpp"
#include "light.hpp"
#include "meshrenderer.hpp"
#include "rigidbody.hpp"
#include "collider.hpp"
#include "terrain.hpp"
#include "gamecamera.hpp"

using namespace Mesh;
using namespace Material;

class Editor;

class SaveLoadSystem {

private:

	Editor* editor;

public:

	SaveLoadSystem(Editor* editor);

	void saveEntitiesRecursively(Transform* parent, rapidxml::xml_document<>& doc, rapidxml::xml_node<>* entityNode);

	void loadEntities();

	void loadEntitiesRecursively(rapidxml::xml_node<>* parentNode, Entity* parent);

	bool saveEntities();

	bool saveSceneCamera();

	bool loadSceneCamera();

	bool saveTransformComponent(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* entNode, Transform* transform);

	bool loadTransformComponent(Entity* ent, rapidxml::xml_node<>* entNode);

	bool saveGameCameraComponent(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* entNode, GameCamera* camera);

	bool loadGameCameraComponent(Entity* ent, rapidxml::xml_node<>* entNode);

	bool saveLightComponent(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* entNode, Light* light);

	bool loadLightComponent(Entity* ent, rapidxml::xml_node<>* entNode);

	bool saveMeshRendererComponent(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* entNode, MeshRenderer* meshRenderer);

	bool loadTerrainGeneratorComponent(Entity* ent, rapidxml::xml_node<>* entNode);

	bool saveTerrainGeneratorComponent(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* entNode, TerrainGenerator* terrain);

	bool loadMeshRendererComponent(Entity* ent, rapidxml::xml_node<>* entNode);

	bool saveRigidbodyComponent(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* entNode, Rigidbody* rigidbody);

	bool loadRigidbodyComponent(Entity* ent, rapidxml::xml_node<>* entNode);

	bool saveMeshColliderComponent(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* entNode, MeshCollider* meshCollider);

	bool loadMeshColliderComponents(Entity* ent, rapidxml::xml_node<>* entNode);

	bool saveBoxColliderComponent(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* entNode, BoxCollider* boxCollider);

	bool loadBoxColliderComponents(Entity* ent, rapidxml::xml_node<>* entNode);

	bool saveSphereColliderComponent(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* entNode, SphereCollider* sphereCollider);

	bool loadSphereColliderComponents(Entity* ent, rapidxml::xml_node<>* entNode);

	bool saveCapsuleColliderComponent(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* entNode, CapsuleCollider* capsuleCollider);

	bool loadCapsuleColliderComponents(Entity* ent, rapidxml::xml_node<>* entNode);
};