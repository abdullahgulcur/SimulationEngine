#include "saveloadsystem.hpp"
#include "editor.hpp"

bool SaveLoadSystem::saveEntities(Editor* editor) {

	rapidxml::xml_document<> doc;
	rapidxml::xml_node<>* decl = doc.allocate_node(rapidxml::node_declaration);
	decl->append_attribute(doc.allocate_attribute("version", "1.0"));
	decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
	doc.append_node(decl);

	rapidxml::xml_node<>* entitiesNode = doc.allocate_node(rapidxml::node_element, "Entities");
	doc.append_node(entitiesNode);

	rapidxml::xml_node<>* entity = doc.allocate_node(rapidxml::node_element, "Entity");
	entity->append_attribute(doc.allocate_attribute("Name", doc.allocate_string(editor->scene.entities[0]->name)));
	saveTransformComponent(doc, entity, editor->scene.entities[0]->transform);
	entitiesNode->append_node(entity);

	SaveLoadSystem::saveEntitiesRecursively(editor, editor->scene.entities[0]->transform, doc, entity);

	std::string xml_as_string;
	rapidxml::print(std::back_inserter(xml_as_string), doc);

	std::ofstream file_stored(editor->fileSystem.assetsPathExternal + "\\MyProject\\Database\\entity.xml");
	file_stored << doc;
	file_stored.close();
	doc.clear();
	return true;
}

void SaveLoadSystem::saveEntitiesRecursively(Editor* editor, Transform* parent, rapidxml::xml_document<>& doc, rapidxml::xml_node<>* entityNode) {

	for (auto& child : parent->children) {

		rapidxml::xml_node<>* entity = doc.allocate_node(rapidxml::node_element, "Entity");
		entity->append_attribute(doc.allocate_attribute("Name", doc.allocate_string(child->entity->name)));

		saveTransformComponent(doc, entity, child);

		if (Light* lightComp = child->entity->getComponent<Light>())
			saveLightComponent(editor, doc, entity, lightComp);

		if (MeshRenderer* meshRendererComp = child->entity->getComponent<MeshRenderer>())
			saveMeshRendererComponent(editor, doc, entity, meshRendererComp);

		if (Rigidbody* rigidbodyComp = child->entity->getComponent<Rigidbody>())
			saveRigidbodyComponent(doc, entity, rigidbodyComp);

		for (auto& comp : child->entity->getComponents<BoxCollider>())
			saveBoxColliderComponent(editor, doc, entity, comp);

		for (auto& comp : child->entity->getComponents<SphereCollider>())
			saveSphereColliderComponent(editor, doc, entity, comp);

		for (auto& comp : child->entity->getComponents<CapsuleCollider>())
			saveCapsuleColliderComponent(editor, doc, entity, comp);

		for (auto& comp : child->entity->getComponents<MeshCollider>())
			saveMeshColliderComponent(editor, doc, entity, comp);

		entityNode->append_node(entity);

		SaveLoadSystem::saveEntitiesRecursively(editor, child, doc, entity);
	}
}

void SaveLoadSystem::loadEntities(Editor* editor) {

	std::ifstream file(editor->fileSystem.assetsPathExternal + "\\MyProject\\Database\\entity.xml");

	if (file.fail()) {

		Entity* ent = new Entity("Root");
		editor->scene.entities.push_back(ent);
		SaveLoadSystem::saveEntities(editor);
		return;
	}

	rapidxml::xml_document<> doc;
	rapidxml::xml_node<>* root_node = NULL;

	std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	buffer.push_back('\0');

	doc.parse<0>(&buffer[0]);

	root_node = doc.first_node("Entities");

	Entity* ent = new Entity("Root");
	editor->scene.entities.push_back(ent);
	ent->transform->updateSelfAndChild();

	SaveLoadSystem::loadEntitiesRecursively(editor, root_node->first_node("Entity"), ent);

	file.close();
}

void SaveLoadSystem::loadEntitiesRecursively(Editor* editor, rapidxml::xml_node<>* parentNode, Entity* parent) {

	for (rapidxml::xml_node<>* entityNode = parentNode->first_node("Entity"); entityNode; entityNode = entityNode->next_sibling()) {

		Entity* ent = new Entity(entityNode->first_attribute("Name")->value(), parent->transform);
		editor->scene.entities.push_back(ent);

		SaveLoadSystem::loadTransformComponent(ent, entityNode);
		SaveLoadSystem::loadLightComponent(editor, ent, entityNode);
		SaveLoadSystem::loadMeshRendererComponent(editor, ent, entityNode);
		SaveLoadSystem::loadBoxColliderComponents(editor, ent, entityNode);
		SaveLoadSystem::loadSphereColliderComponents(editor, ent, entityNode);
		SaveLoadSystem::loadCapsuleColliderComponents(editor, ent, entityNode);
		SaveLoadSystem::loadMeshColliderComponents(editor, ent, entityNode);
		SaveLoadSystem::loadRigidbodyComponent(editor, ent, entityNode);

		SaveLoadSystem::loadEntitiesRecursively(editor, entityNode, ent);
	}
}

bool SaveLoadSystem::saveSceneCamera(Editor* editor) {

	rapidxml::xml_document<> doc;
	rapidxml::xml_node<>* decl = doc.allocate_node(rapidxml::node_declaration);
	decl->append_attribute(doc.allocate_attribute("version", "1.0"));
	decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
	doc.append_node(decl);

	rapidxml::xml_node<>* cameraNode = doc.allocate_node(rapidxml::node_element, "SceneCamera");
	doc.append_node(cameraNode);

	rapidxml::xml_node<>* positionNode = doc.allocate_node(rapidxml::node_element, "Position");
	positionNode->append_attribute(doc.allocate_attribute("X", doc.allocate_string(std::to_string(editor->editorCamera.position.x).c_str())));
	positionNode->append_attribute(doc.allocate_attribute("Y", doc.allocate_string(std::to_string(editor->editorCamera.position.y).c_str())));
	positionNode->append_attribute(doc.allocate_attribute("Z", doc.allocate_string(std::to_string(editor->editorCamera.position.z).c_str())));
	cameraNode->append_node(positionNode);

	rapidxml::xml_node<>* angleNode = doc.allocate_node(rapidxml::node_element, "Angle");
	angleNode->append_attribute(doc.allocate_attribute("Horizontal", doc.allocate_string(std::to_string(editor->editorCamera.horizontalAngle).c_str())));
	angleNode->append_attribute(doc.allocate_attribute("Vertical", doc.allocate_string(std::to_string(editor->editorCamera.verticalAngle).c_str())));
	cameraNode->append_node(angleNode);

	std::string xml_as_string;
	rapidxml::print(std::back_inserter(xml_as_string), doc);

	std::ofstream file_stored(editor->fileSystem.assetsPathExternal + "\\MyProject\\Database\\scenecamera_db.xml");
	file_stored << doc;
	file_stored.close();
	doc.clear();
	return true;
}

bool SaveLoadSystem::loadSceneCamera(Editor* editor) {

	std::ifstream file(editor->fileSystem.assetsPathExternal + "\\MyProject\\Database\\scenecamera_db.xml");

	if (file.fail())
		return false;

	rapidxml::xml_document<> doc;
	rapidxml::xml_node<>* root_node = NULL;

	std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	buffer.push_back('\0');

	doc.parse<0>(&buffer[0]);

	root_node = doc.first_node("SceneCamera");

	editor->editorCamera.position.x = atof(root_node->first_node("Position")->first_attribute("X")->value());
	editor->editorCamera.position.y = atof(root_node->first_node("Position")->first_attribute("Y")->value());
	editor->editorCamera.position.z = atof(root_node->first_node("Position")->first_attribute("Z")->value());

	editor->editorCamera.horizontalAngle = atof(root_node->first_node("Angle")->first_attribute("Horizontal")->value());
	editor->editorCamera.verticalAngle = atof(root_node->first_node("Angle")->first_attribute("Vertical")->value());

	file.close();
	return true;
}

bool SaveLoadSystem::saveTransformComponent(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* entNode, Transform* transform) {

	rapidxml::xml_node<>* transformNode = doc.allocate_node(rapidxml::node_element, "Transform");

	rapidxml::xml_node<>* positionNode = doc.allocate_node(rapidxml::node_element, "Position");
	positionNode->append_attribute(doc.allocate_attribute("X", doc.allocate_string(std::to_string(transform->localPosition.x).c_str())));
	positionNode->append_attribute(doc.allocate_attribute("Y", doc.allocate_string(std::to_string(transform->localPosition.y).c_str())));
	positionNode->append_attribute(doc.allocate_attribute("Z", doc.allocate_string(std::to_string(transform->localPosition.z).c_str())));
	transformNode->append_node(positionNode);

	rapidxml::xml_node<>* rotationNode = doc.allocate_node(rapidxml::node_element, "Rotation");
	rotationNode->append_attribute(doc.allocate_attribute("X", doc.allocate_string(std::to_string(transform->localRotation.x).c_str())));
	rotationNode->append_attribute(doc.allocate_attribute("Y", doc.allocate_string(std::to_string(transform->localRotation.y).c_str())));
	rotationNode->append_attribute(doc.allocate_attribute("Z", doc.allocate_string(std::to_string(transform->localRotation.z).c_str())));
	transformNode->append_node(rotationNode);

	rapidxml::xml_node<>* scaleNode = doc.allocate_node(rapidxml::node_element, "Scale");
	scaleNode->append_attribute(doc.allocate_attribute("X", doc.allocate_string(std::to_string(transform->localScale.x).c_str())));
	scaleNode->append_attribute(doc.allocate_attribute("Y", doc.allocate_string(std::to_string(transform->localScale.y).c_str())));
	scaleNode->append_attribute(doc.allocate_attribute("Z", doc.allocate_string(std::to_string(transform->localScale.z).c_str())));
	transformNode->append_node(scaleNode);

	entNode->append_node(transformNode);

	return true;
}

bool SaveLoadSystem::loadTransformComponent(Entity* ent, rapidxml::xml_node<>* entNode) {

	rapidxml::xml_node<>* transform_node = entNode->first_node("Transform");

	ent->transform->localPosition.x = atof(transform_node->first_node("Position")->first_attribute("X")->value());
	ent->transform->localPosition.y = atof(transform_node->first_node("Position")->first_attribute("Y")->value());
	ent->transform->localPosition.z = atof(transform_node->first_node("Position")->first_attribute("Z")->value());

	ent->transform->localRotation.x = atof(transform_node->first_node("Rotation")->first_attribute("X")->value());
	ent->transform->localRotation.y = atof(transform_node->first_node("Rotation")->first_attribute("Y")->value());
	ent->transform->localRotation.z = atof(transform_node->first_node("Rotation")->first_attribute("Z")->value());

	ent->transform->localScale.x = atof(transform_node->first_node("Scale")->first_attribute("X")->value());
	ent->transform->localScale.y = atof(transform_node->first_node("Scale")->first_attribute("Y")->value());
	ent->transform->localScale.z = atof(transform_node->first_node("Scale")->first_attribute("Z")->value());

	ent->transform->updateSelfAndChild();

	return true;
}

bool SaveLoadSystem::saveLightComponent(Editor* editor, rapidxml::xml_document<>& doc, rapidxml::xml_node<>* entNode, Light* light) {

	rapidxml::xml_node<>* lightNode = doc.allocate_node(rapidxml::node_element, "Light");
	lightNode->append_attribute(doc.allocate_attribute("Type", doc.allocate_string(editor->scene.getLightType(light->lightType).c_str())));
	lightNode->append_attribute(doc.allocate_attribute("Power", doc.allocate_string(std::to_string(light->power).c_str())));

	rapidxml::xml_node<>* colorNode = doc.allocate_node(rapidxml::node_element, "Color");
	colorNode->append_attribute(doc.allocate_attribute("X", doc.allocate_string(std::to_string(light->color.x).c_str())));
	colorNode->append_attribute(doc.allocate_attribute("Y", doc.allocate_string(std::to_string(light->color.y).c_str())));
	colorNode->append_attribute(doc.allocate_attribute("Z", doc.allocate_string(std::to_string(light->color.z).c_str())));
	lightNode->append_node(colorNode);

	entNode->append_node(lightNode);

	return true;
}

bool SaveLoadSystem::loadLightComponent(Editor* editor, Entity* ent, rapidxml::xml_node<>* entNode) {

	rapidxml::xml_node<>* lightNode = entNode->first_node("Light");

	if (lightNode == NULL)
		return false;

	Light* lightComp = ent->addComponent<Light>();

	lightComp->power = atof(lightNode->first_attribute("Power")->value());
	lightComp->color.x = atof(lightNode->first_node("Color")->first_attribute("X")->value());
	lightComp->color.y = atof(lightNode->first_node("Color")->first_attribute("Y")->value());
	lightComp->color.z = atof(lightNode->first_node("Color")->first_attribute("Z")->value());

	if (strcmp(lightNode->first_attribute("Type")->value(), "Point") == 0)
		lightComp->lightType = LightType::PointLight;
	else 
		lightComp->lightType = LightType::DirectionalLight;

	return true;
}

bool SaveLoadSystem::saveMeshRendererComponent(Editor* editor, rapidxml::xml_document<>& doc, rapidxml::xml_node<>* entNode, MeshRenderer* meshRenderer) {

	rapidxml::xml_node<>* meshRendererNode = doc.allocate_node(rapidxml::node_element, "MeshRenderer");

	if(meshRenderer->mesh->fileAddr == NULL)
		meshRendererNode->append_attribute(doc.allocate_attribute("MeshPath", doc.allocate_string("NULL")));
	else
		meshRendererNode->append_attribute(doc.allocate_attribute("MeshPath", doc.allocate_string(editor->fileSystem.files[meshRenderer->mesh->fileAddr->id].path.c_str())));

	if (meshRenderer->mat->fileAddr == NULL)
		meshRendererNode->append_attribute(doc.allocate_attribute("MaterialPath", doc.allocate_string("Default")));
	else
		meshRendererNode->append_attribute(doc.allocate_attribute("MaterialPath", doc.allocate_string(editor->fileSystem.files[meshRenderer->mat->fileAddr->id].path.c_str())));

	entNode->append_node(meshRendererNode);

	return true;
}

bool SaveLoadSystem::loadMeshRendererComponent(Editor* editor, Entity* ent, rapidxml::xml_node<>* entNode) {

	rapidxml::xml_node<>* meshRendererNode = entNode->first_node("MeshRenderer");

	if (meshRendererNode == NULL)
		return false;

	MeshRenderer* meshRendererComp = ent->addComponent<MeshRenderer>();

	auto meshFound = editor->fileSystem.meshes.find(meshRendererNode->first_attribute("MeshPath")->value());

	if (meshFound != editor->fileSystem.meshes.end())
		meshRendererComp->mesh = &meshFound->second;
	else
		meshRendererComp->mesh = &editor->fileSystem.meshes["Null"];

	meshRendererComp->mesh->meshRendererCompAddrs.push_back(meshRendererComp);


	auto matFound = editor->fileSystem.materials.find(meshRendererNode->first_attribute("MaterialPath")->value());
	if (matFound != editor->fileSystem.materials.end())
		meshRendererComp->mat = &matFound->second;
	else
		meshRendererComp->mat = &editor->fileSystem.materials["Default"];

	meshRendererComp->mat->meshRendererCompAddrs.push_back(meshRendererComp);

	return true;
}

bool SaveLoadSystem::saveRigidbodyComponent(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* entNode, Rigidbody* rigidbody) {

	rapidxml::xml_node<>* rigidbodyNode = doc.allocate_node(rapidxml::node_element, "Rigidbody");
	rigidbodyNode->append_attribute(doc.allocate_attribute("Mass", doc.allocate_string(std::to_string(rigidbody->body->getMass()).c_str())));
	rigidbodyNode->append_attribute(doc.allocate_attribute("UseGravity", doc.allocate_string(std::to_string(rigidbody->body->getActorFlags() & PxActorFlag::eDISABLE_GRAVITY ? 0 : 1).c_str())));
	rigidbodyNode->append_attribute(doc.allocate_attribute("IsKinematic", doc.allocate_string(std::to_string(rigidbody->body->getRigidBodyFlags() & PxRigidBodyFlag::eKINEMATIC ? 1 : 0).c_str())));

	rapidxml::xml_node<>* lockPositionNode = doc.allocate_node(rapidxml::node_element, "LockPosition");
	int lockPosX = rigidbody->body->getRigidDynamicLockFlags() & PxRigidDynamicLockFlag::eLOCK_LINEAR_X ? 1 : 0;
	int lockPosY = rigidbody->body->getRigidDynamicLockFlags() & PxRigidDynamicLockFlag::eLOCK_LINEAR_Y ? 1 : 0;
	int lockPosZ = rigidbody->body->getRigidDynamicLockFlags() & PxRigidDynamicLockFlag::eLOCK_LINEAR_Z ? 1 : 0;
	lockPositionNode->append_attribute(doc.allocate_attribute("X", doc.allocate_string(std::to_string(lockPosX).c_str())));
	lockPositionNode->append_attribute(doc.allocate_attribute("Y", doc.allocate_string(std::to_string(lockPosY).c_str())));
	lockPositionNode->append_attribute(doc.allocate_attribute("Z", doc.allocate_string(std::to_string(lockPosZ).c_str())));
	rigidbodyNode->append_node(lockPositionNode);

	rapidxml::xml_node<>* lockRotationNode = doc.allocate_node(rapidxml::node_element, "LockRotation");
	int lockRotX = rigidbody->body->getRigidDynamicLockFlags() & PxRigidDynamicLockFlag::eLOCK_ANGULAR_X ? 1 : 0;
	int lockRotY = rigidbody->body->getRigidDynamicLockFlags() & PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y ? 1 : 0;
	int lockRotZ = rigidbody->body->getRigidDynamicLockFlags() & PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z ? 1 : 0;
	lockRotationNode->append_attribute(doc.allocate_attribute("X", doc.allocate_string(std::to_string(lockRotX).c_str())));
	lockRotationNode->append_attribute(doc.allocate_attribute("Y", doc.allocate_string(std::to_string(lockRotY).c_str())));
	lockRotationNode->append_attribute(doc.allocate_attribute("Z", doc.allocate_string(std::to_string(lockRotZ).c_str())));
	rigidbodyNode->append_node(lockRotationNode);

	entNode->append_node(rigidbodyNode);

	return true;
}

bool SaveLoadSystem::loadRigidbodyComponent(Editor* editor, Entity* ent, rapidxml::xml_node<>* entNode) {

	rapidxml::xml_node<>* rigidbodyNode = entNode->first_node("Rigidbody");

	if (rigidbodyNode == NULL)
		return false;

	glm::quat myquaternion = glm::quat(ent->transform->globalRotation);
	PxTransform tm(PxVec3(ent->transform->globalPosition.x, ent->transform->globalPosition.y,
		ent->transform->globalPosition.z),
		PxQuat(myquaternion.x, myquaternion.y, myquaternion.z, myquaternion.w));

	Rigidbody* rigidbodyComp = ent->addComponent<Rigidbody>();
	rigidbodyComp->body = editor->physics.gPhysics->createRigidDynamic(tm);
	rigidbodyComp->body->setMass(atof(rigidbodyNode->first_attribute("Mass")->value()));
	rigidbodyComp->body->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !(atoi(rigidbodyNode->first_attribute("UseGravity")->value()) == 1 ? true : false));
	rigidbodyComp->body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, atoi(rigidbodyNode->first_attribute("IsKinematic")->value()) == 1 ? true : false);

	rapidxml::xml_node<>* lockPositionNode = rigidbodyNode->first_node("LockPosition");
	rigidbodyComp->body->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_X, atoi(lockPositionNode->first_attribute("X")->value()) == 1 ? true : false);
	rigidbodyComp->body->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, atoi(lockPositionNode->first_attribute("Y")->value()) == 1 ? true : false);
	rigidbodyComp->body->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, atoi(lockPositionNode->first_attribute("Z")->value()) == 1 ? true : false);

	rapidxml::xml_node<>* lockRotationNode = rigidbodyNode->first_node("LockRotation");
	rigidbodyComp->body->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, atoi(lockRotationNode->first_attribute("X")->value()) == 1 ? true : false);
	rigidbodyComp->body->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, atoi(lockRotationNode->first_attribute("Y")->value()) == 1 ? true : false);
	rigidbodyComp->body->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, atoi(lockRotationNode->first_attribute("Z")->value()) == 1 ? true : false);

	editor->physics.gScene->addActor(*rigidbodyComp->body);

	for (auto& comp : ent->getComponents<Collider>()) {
		rigidbodyComp->body->attachShape(*comp->shape);
		comp->shape->release();
	}

	return true;
}

bool SaveLoadSystem::saveMeshColliderComponent(Editor* editor, rapidxml::xml_document<>& doc, rapidxml::xml_node<>* entNode, MeshCollider* meshCollider) {

	rapidxml::xml_node<>* meshColliderNode = doc.allocate_node(rapidxml::node_element, "MeshCollider");
	meshColliderNode->append_attribute(doc.allocate_attribute("Convex", doc.allocate_string(
		std::to_string(meshCollider->shape->getGeometryType() == PxGeometryType::eCONVEXMESH ? 1 : 0).c_str())));

	int trigger = meshCollider->shape->getFlags() & PxShapeFlag::eTRIGGER_SHAPE ? 1 : 0;
	meshColliderNode->append_attribute(doc.allocate_attribute("Trigger", doc.allocate_string(std::to_string(trigger).c_str())));

	rapidxml::xml_node<>* physicMatNode = doc.allocate_node(rapidxml::node_element, "PhysicMaterial");
	physicMatNode->append_attribute(doc.allocate_attribute("Path", doc.allocate_string((editor->fileSystem.getPhysicMaterialPath(meshCollider->pmat->fileAddr)))));

	meshColliderNode->append_node(physicMatNode);

	entNode->append_node(meshColliderNode);

	return true;
}

bool SaveLoadSystem::loadMeshColliderComponents(Editor* editor, Entity* ent, rapidxml::xml_node<>* entNode) {

	bool b = (5 & 1);
	std::cout << "test et " << (5 & 5) << std::endl;

	for (rapidxml::xml_node<>* meshColliderNode = entNode->first_node("MeshCollider"); meshColliderNode; meshColliderNode = meshColliderNode->next_sibling()) {

		if (strcmp(meshColliderNode->name(), "MeshCollider") != 0)
			continue;

		MeshCollider* meshColliderComp = ent->addComponent<MeshCollider>();

		const char* pmatFilePath = meshColliderNode->first_node("PhysicMaterial")->first_attribute("Path")->value();
		File* pmatFileAddr = editor->fileSystem.getPhysicMaterialAddr(pmatFilePath);

		if (pmatFileAddr == NULL)
			pmatFilePath = "Default";

		meshColliderComp->pmat = &editor->fileSystem.physicmaterials[pmatFilePath];
		meshColliderComp->pmat->colliderCompAddrs.push_back(meshColliderComp);

		bool convex = atoi(meshColliderNode->first_attribute("Convex")->value()) == 1;

		if (convex) {

			PxConvexMesh* convexMesh = meshColliderComp->createConvexMesh(ent->getComponent<MeshRenderer>(),
				editor->physics.gPhysics, editor->physics.gCooking);
			meshColliderComp->shape = editor->physics.gPhysics->createShape(PxConvexMeshGeometry(convexMesh,
				PxVec3(ent->transform->globalScale.x, ent->transform->globalScale.y, ent->transform->globalScale.z)), *meshColliderComp->pmat->pxmat, true);
		}
		else {

			PxTriangleMesh* triangleMesh = meshColliderComp->createTriangleMesh(ent->getComponent<MeshRenderer>(),
				editor->physics.gPhysics, editor->physics.gCooking);
			meshColliderComp->shape = editor->physics.gPhysics->createShape(PxTriangleMeshGeometry(triangleMesh,
				PxVec3(ent->transform->globalScale.x, ent->transform->globalScale.y, ent->transform->globalScale.z)), *meshColliderComp->pmat->pxmat, true);
		}

		bool isTrigger = atoi(meshColliderNode->first_attribute("Trigger")->value()) == 1 ? true : false;
		meshColliderComp->shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !isTrigger);
		meshColliderComp->shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, isTrigger);
	}


	return true;
}

bool SaveLoadSystem::saveBoxColliderComponent(Editor* editor, rapidxml::xml_document<>& doc, rapidxml::xml_node<>* entNode, BoxCollider* boxCollider) {

	rapidxml::xml_node<>* boxColliderNode = doc.allocate_node(rapidxml::node_element, "BoxCollider");
	int trigger = boxCollider->shape->getFlags() & PxShapeFlag::eTRIGGER_SHAPE ? 1 : 0;
	boxColliderNode->append_attribute(doc.allocate_attribute("Trigger", doc.allocate_string(std::to_string(trigger).c_str())));

	rapidxml::xml_node<>* centerNode = doc.allocate_node(rapidxml::node_element, "Center");
	centerNode->append_attribute(doc.allocate_attribute("X", doc.allocate_string(std::to_string(boxCollider->center.x).c_str())));
	centerNode->append_attribute(doc.allocate_attribute("Y", doc.allocate_string(std::to_string(boxCollider->center.y).c_str())));
	centerNode->append_attribute(doc.allocate_attribute("Z", doc.allocate_string(std::to_string(boxCollider->center.z).c_str())));
	boxColliderNode->append_node(centerNode);

	rapidxml::xml_node<>* sizeNode = doc.allocate_node(rapidxml::node_element, "Size");
	sizeNode->append_attribute(doc.allocate_attribute("X", doc.allocate_string(std::to_string(boxCollider->size.x).c_str())));
	sizeNode->append_attribute(doc.allocate_attribute("Y", doc.allocate_string(std::to_string(boxCollider->size.y).c_str())));
	sizeNode->append_attribute(doc.allocate_attribute("Z", doc.allocate_string(std::to_string(boxCollider->size.z).c_str())));
	boxColliderNode->append_node(sizeNode);

	rapidxml::xml_node<>* physicMatNode = doc.allocate_node(rapidxml::node_element, "PhysicMaterial");
	physicMatNode->append_attribute(doc.allocate_attribute("Path", doc.allocate_string((editor->fileSystem.getPhysicMaterialPath(boxCollider->pmat->fileAddr)))));

	boxColliderNode->append_node(physicMatNode);

	entNode->append_node(boxColliderNode);

	return true;
}

bool SaveLoadSystem::loadBoxColliderComponents(Editor* editor, Entity* ent, rapidxml::xml_node<>* entNode) {

	for (rapidxml::xml_node<>* boxColliderNode = entNode->first_node("BoxCollider"); boxColliderNode; boxColliderNode = boxColliderNode->next_sibling()) {

		if (strcmp(boxColliderNode->name(), "BoxCollider") != 0)
			continue;

		BoxCollider* boxColliderComp = ent->addComponent<BoxCollider>();

		const char* pmatFilePath = boxColliderNode->first_node("PhysicMaterial")->first_attribute("Path")->value();
		File* pmatFileAddr = editor->fileSystem.getPhysicMaterialAddr(pmatFilePath);

		if (pmatFileAddr == NULL)
			pmatFilePath = "Default";

		boxColliderComp->pmat = &editor->fileSystem.physicmaterials[pmatFilePath];
		boxColliderComp->pmat->colliderCompAddrs.push_back(boxColliderComp);

		bool isTrigger = atoi(boxColliderNode->first_attribute("Trigger")->value()) == 1 ? true : false;

		boxColliderComp->center.x = atof(boxColliderNode->first_node("Center")->first_attribute("X")->value());
		boxColliderComp->center.y = atof(boxColliderNode->first_node("Center")->first_attribute("Y")->value());
		boxColliderComp->center.z = atof(boxColliderNode->first_node("Center")->first_attribute("Z")->value());

		boxColliderComp->size.x = atof(boxColliderNode->first_node("Size")->first_attribute("X")->value());
		boxColliderComp->size.y = atof(boxColliderNode->first_node("Size")->first_attribute("Y")->value());
		boxColliderComp->size.z = atof(boxColliderNode->first_node("Size")->first_attribute("Z")->value());

		glm::vec3 size = ent->transform->globalScale * boxColliderComp->size / 2.f;
		boxColliderComp->shape = editor->physics.gPhysics->createShape(PxBoxGeometry(size.x, size.y, size.z), *boxColliderComp->pmat->pxmat, true);
		boxColliderComp->shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !isTrigger);
		boxColliderComp->shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, isTrigger);
		glm::vec3 center = boxColliderComp->transform->globalScale * boxColliderComp->center;
		boxColliderComp->shape->setLocalPose(PxTransform(center.x, center.y, center.z));
	}

	return true;
}

bool SaveLoadSystem::saveSphereColliderComponent(Editor* editor, rapidxml::xml_document<>& doc, rapidxml::xml_node<>* entNode, SphereCollider* sphereCollider) {

	rapidxml::xml_node<>* sphereColliderNode = doc.allocate_node(rapidxml::node_element, "SphereCollider");
	int trigger = sphereCollider->shape->getFlags() & PxShapeFlag::eTRIGGER_SHAPE ? 1 : 0;
	sphereColliderNode->append_attribute(doc.allocate_attribute("Trigger", doc.allocate_string(std::to_string(trigger).c_str())));
	sphereColliderNode->append_attribute(doc.allocate_attribute("Radius", doc.allocate_string(std::to_string(sphereCollider->radius).c_str())));

	rapidxml::xml_node<>* centerNode = doc.allocate_node(rapidxml::node_element, "Center");
	centerNode->append_attribute(doc.allocate_attribute("X", doc.allocate_string(std::to_string(sphereCollider->center.x).c_str())));
	centerNode->append_attribute(doc.allocate_attribute("Y", doc.allocate_string(std::to_string(sphereCollider->center.y).c_str())));
	centerNode->append_attribute(doc.allocate_attribute("Z", doc.allocate_string(std::to_string(sphereCollider->center.z).c_str())));
	sphereColliderNode->append_node(centerNode);

	rapidxml::xml_node<>* physicMatNode = doc.allocate_node(rapidxml::node_element, "PhysicMaterial");
	physicMatNode->append_attribute(doc.allocate_attribute("Path", doc.allocate_string((editor->fileSystem.getPhysicMaterialPath(sphereCollider->pmat->fileAddr)))));

	sphereColliderNode->append_node(physicMatNode);
	entNode->append_node(sphereColliderNode);

	return true;
}

bool SaveLoadSystem::loadSphereColliderComponents(Editor* editor, Entity* ent, rapidxml::xml_node<>* entNode) {

	for (rapidxml::xml_node<>* sphereColliderNode = entNode->first_node("SphereCollider"); sphereColliderNode; sphereColliderNode = sphereColliderNode->next_sibling()) {

		if (strcmp(sphereColliderNode->name(), "SphereCollider") != 0)
			continue;

		SphereCollider* sphereColliderComp = ent->addComponent<SphereCollider>();

		const char* pmatFilePath = sphereColliderNode->first_node("PhysicMaterial")->first_attribute("Path")->value();
		File* pmatFileAddr = editor->fileSystem.getPhysicMaterialAddr(pmatFilePath);

		if (pmatFileAddr == NULL)
			pmatFilePath = "Default";

		sphereColliderComp->pmat = &editor->fileSystem.physicmaterials[pmatFilePath];
		sphereColliderComp->pmat->colliderCompAddrs.push_back(sphereColliderComp);

		bool isTrigger = atoi(sphereColliderNode->first_attribute("Trigger")->value()) == 1 ? true : false;
		
		sphereColliderComp->radius = atof(sphereColliderNode->first_attribute("Radius")->value());

		sphereColliderComp->center.x = atof(sphereColliderNode->first_node("Center")->first_attribute("X")->value());
		sphereColliderComp->center.y = atof(sphereColliderNode->first_node("Center")->first_attribute("Y")->value());
		sphereColliderComp->center.z = atof(sphereColliderNode->first_node("Center")->first_attribute("Z")->value());

		float max = ent->transform->globalScale.x > ent->transform->globalScale.y ? ent->transform->globalScale.x : ent->transform->globalScale.y;
		max = max > ent->transform->globalScale.z ? max : ent->transform->globalScale.z;

		sphereColliderComp->shape = editor->physics.gPhysics->createShape(PxSphereGeometry(sphereColliderComp->radius * max), *sphereColliderComp->pmat->pxmat, true);
		sphereColliderComp->shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !isTrigger);
		sphereColliderComp->shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, isTrigger);
		glm::vec3 center = sphereColliderComp->transform->globalScale * sphereColliderComp->center;
		sphereColliderComp->shape->setLocalPose(PxTransform(center.x, center.y, center.z));
	}

	return true;
}

bool SaveLoadSystem::saveCapsuleColliderComponent(Editor* editor, rapidxml::xml_document<>& doc, rapidxml::xml_node<>* entNode, CapsuleCollider* capsuleCollider) {

	rapidxml::xml_node<>* capsuleColliderNode = doc.allocate_node(rapidxml::node_element, "CapsuleCollider");
	int trigger = capsuleCollider->shape->getFlags() & PxShapeFlag::eTRIGGER_SHAPE ? 1 : 0;
	capsuleColliderNode->append_attribute(doc.allocate_attribute("Trigger", doc.allocate_string(std::to_string(trigger).c_str())));
	capsuleColliderNode->append_attribute(doc.allocate_attribute("Radius", doc.allocate_string(std::to_string(capsuleCollider->radius).c_str())));
	capsuleColliderNode->append_attribute(doc.allocate_attribute("Height", doc.allocate_string(std::to_string(capsuleCollider->height).c_str())));
	capsuleColliderNode->append_attribute(doc.allocate_attribute("Axis", doc.allocate_string(std::to_string(capsuleCollider->getAxis()).c_str())));

	rapidxml::xml_node<>* centerNode = doc.allocate_node(rapidxml::node_element, "Center");
	centerNode->append_attribute(doc.allocate_attribute("X", doc.allocate_string(std::to_string(capsuleCollider->center.x).c_str())));
	centerNode->append_attribute(doc.allocate_attribute("Y", doc.allocate_string(std::to_string(capsuleCollider->center.y).c_str())));
	centerNode->append_attribute(doc.allocate_attribute("Z", doc.allocate_string(std::to_string(capsuleCollider->center.z).c_str())));
	capsuleColliderNode->append_node(centerNode);

	rapidxml::xml_node<>* physicMatNode = doc.allocate_node(rapidxml::node_element, "PhysicMaterial");
	physicMatNode->append_attribute(doc.allocate_attribute("Path", doc.allocate_string((editor->fileSystem.getPhysicMaterialPath(capsuleCollider->pmat->fileAddr)))));

	capsuleColliderNode->append_node(physicMatNode);
	entNode->append_node(capsuleColliderNode);

	return true;
}

bool SaveLoadSystem::loadCapsuleColliderComponents(Editor* editor, Entity* ent, rapidxml::xml_node<>* entNode) {

	for (rapidxml::xml_node<>* capsuleColliderNode = entNode->first_node("CapsuleCollider"); capsuleColliderNode; capsuleColliderNode = capsuleColliderNode->next_sibling()) {

		if (strcmp(capsuleColliderNode->name(), "CapsuleCollider") != 0)
			continue;

		CapsuleCollider* capsuleColliderComp = ent->addComponent<CapsuleCollider>();

		const char* pmatFilePath = capsuleColliderNode->first_node("PhysicMaterial")->first_attribute("Path")->value();
		File* pmatFileAddr = editor->fileSystem.getPhysicMaterialAddr(pmatFilePath);

		if (pmatFileAddr == NULL)
			pmatFilePath = "Default";

		capsuleColliderComp->pmat = &editor->fileSystem.physicmaterials[pmatFilePath];
		capsuleColliderComp->pmat->colliderCompAddrs.push_back(capsuleColliderComp);

		bool isTrigger = atoi(capsuleColliderNode->first_attribute("Trigger")->value()) == 1 ? true : false;

		capsuleColliderComp->radius = atof(capsuleColliderNode->first_attribute("Radius")->value());
		capsuleColliderComp->height = atof(capsuleColliderNode->first_attribute("Height")->value());
		capsuleColliderComp->setAxis(atoi(capsuleColliderNode->first_attribute("Axis")->value()));

		capsuleColliderComp->center.x = atof(capsuleColliderNode->first_node("Center")->first_attribute("X")->value());
		capsuleColliderComp->center.y = atof(capsuleColliderNode->first_node("Center")->first_attribute("Y")->value());
		capsuleColliderComp->center.z = atof(capsuleColliderNode->first_node("Center")->first_attribute("Z")->value());

		capsuleColliderComp->shape = editor->physics.gPhysics->createShape(PxCapsuleGeometry(capsuleColliderComp->radius,
			capsuleColliderComp->height / 2), *capsuleColliderComp->pmat->pxmat, true);
		capsuleColliderComp->shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, !isTrigger);
		capsuleColliderComp->shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, isTrigger);
		glm::ivec3 rotVec = capsuleColliderComp->getRotationVector(capsuleColliderComp->getAxis());
		PxVec3 axis(rotVec.x, rotVec.y, rotVec.z);
		PxTransform relativePose(PxVec3(capsuleColliderComp->center.x, capsuleColliderComp->center.y, capsuleColliderComp->center.z), PxQuat(PxHalfPi, axis));
		capsuleColliderComp->shape->setLocalPose(relativePose);
	}

	return true;
}
