#pragma once

#include <string>
#include <queue>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

#include "rapidxml_print.hpp"
#include "rapidxml.hpp"

#include "entity.hpp"
#include "transform.hpp"
#include "light.hpp"
#include "meshrenderer.hpp"
#include "rigidbody.hpp"
#include "collider.hpp"

using namespace Mesh;
using namespace Material;

class Editor;

namespace SaveLoadSystem {

	bool loadSceneGraph(Editor* editor, std::map<int, std::vector<int>>& initialSceneGrap);

	bool saveSceneGraph(Editor* editor);

	bool writeSceneGraphFileRecursively(std::queue<Transform*> entQueue, std::ostringstream& fileTextStream);

	bool loadEntities(Editor* editor);

	bool saveEntities(Editor* editor);
	
	bool saveSceneCamera(Editor* editor);

	bool loadSceneCamera(Editor* editor);

	bool saveTransformComponent(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* entNode, Transform* transform);

	bool loadTransformComponent(Entity* ent, rapidxml::xml_node<>* entNode);

	bool saveLightComponent(Editor* editor, rapidxml::xml_document<>& doc, rapidxml::xml_node<>* entNode, Light* light);

	bool loadLightComponent(Editor* editor, Entity* ent, rapidxml::xml_node<>* entNode);

	bool saveMeshRendererComponent(Editor* editor, rapidxml::xml_document<>& doc, rapidxml::xml_node<>* entNode, MeshRenderer* meshRenderer);

	bool loadMeshRendererComponent(Editor* editor, Entity* ent, rapidxml::xml_node<>* entNode);

	bool saveRigidbodyComponent(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* entNode, Rigidbody* rigidbody);

	bool loadRigidbodyComponent(Editor* editor, Entity* ent, rapidxml::xml_node<>* entNode);

	bool saveMeshColliderComponent(Editor* editor, rapidxml::xml_document<>& doc, rapidxml::xml_node<>* entNode, MeshCollider* meshCollider);

	bool loadMeshColliderComponents(Editor* editor, Entity* ent, rapidxml::xml_node<>* entNode);

	bool saveBoxColliderComponent(Editor* editor, rapidxml::xml_document<>& doc, rapidxml::xml_node<>* entNode, BoxCollider* boxCollider);

	bool loadBoxColliderComponents(Editor* editor, Entity* ent, rapidxml::xml_node<>* entNode);

	bool saveSphereColliderComponent(Editor* editor, rapidxml::xml_document<>& doc, rapidxml::xml_node<>* entNode, SphereCollider* sphereCollider);

	bool loadSphereColliderComponents(Editor* editor, Entity* ent, rapidxml::xml_node<>* entNode);

	bool saveCapsuleColliderComponent(Editor* editor, rapidxml::xml_document<>& doc, rapidxml::xml_node<>* entNode, CapsuleCollider* capsuleCollider);

	bool loadCapsuleColliderComponents(Editor* editor, Entity* ent, rapidxml::xml_node<>* entNode);
}