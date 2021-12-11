#include "saveloadsystem.hpp"
#include "editor.hpp"

bool SaveLoadSystem::loadSceneGraph(Editor* editor, std::map<int, std::vector<int>>& initialSceneGraph) {

	std::ifstream file(editor->fileSystem.assetsPathExternal + "\\MyProject\\Database\\scenegraph.txt");

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
	return true;
}

bool SaveLoadSystem::saveSceneGraph(Editor* editor) {

	std::ofstream file(editor->fileSystem.assetsPathExternal + "\\MyProject\\Database\\scenegraph.txt");

	if (file.fail())
		return false;

	std::ostringstream fileTextStream;

	std::queue<Transform*> entQueue;
	entQueue.push(editor->scene.entities[0].transform);
	SaveLoadSystem::writeSceneGraphFileRecursively(entQueue, fileTextStream);

	file << fileTextStream.str();
	file.close();
	return true;
}

bool SaveLoadSystem::writeSceneGraphFileRecursively(std::queue<Transform*> entQueue, std::ostringstream& fileTextStream) {

	if (entQueue.size() == 0)
		return false;

	Transform* entity = entQueue.front();
	entQueue.pop();

	fileTextStream << entity->id << ' ';

	for (int i = 0; i < entity->children.size(); i++) {

		fileTextStream << entity->children[i]->id << ' ';
		entQueue.push(entity->children[i]);
	}
	fileTextStream << '\n';
	SaveLoadSystem::writeSceneGraphFileRecursively(entQueue, fileTextStream);
	return true;
}

bool SaveLoadSystem::saveEntities(Editor* editor) {

	rapidxml::xml_document<> doc;
	rapidxml::xml_node<>* decl = doc.allocate_node(rapidxml::node_declaration);
	decl->append_attribute(doc.allocate_attribute("version", "1.0"));
	decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
	doc.append_node(decl);

	rapidxml::xml_node<>* entitiesNode = doc.allocate_node(rapidxml::node_element, "Entities");
	doc.append_node(entitiesNode);

	for (Entity& ent : editor->scene.entities) {

		rapidxml::xml_node<>* entity = doc.allocate_node(rapidxml::node_element, "Entity");
		entity->append_attribute(doc.allocate_attribute("Name", doc.allocate_string(ent.name)));

		saveTransformComponent(doc, entity, ent.transform);

		if(Light* lightComp = ent.getComponent<Light>())
			saveLightComponent(editor, doc, entity, lightComp);

		if (MeshRenderer* meshRendererComp = ent.getComponent<MeshRenderer>())
			saveMeshRendererComponent(editor, doc, entity, meshRendererComp);

		if (Rigidbody* rigidbodyComp = ent.getComponent<Rigidbody>())
			saveRigidbodyComponent(doc, entity, rigidbodyComp);

		if (MeshCollider* meshColliderComp = ent.getComponent<MeshCollider>())
			saveMeshColliderComponent(doc, entity, meshColliderComp);

		if (BoxCollider* boxColliderComp = ent.getComponent<BoxCollider>())
			saveBoxColliderComponent(doc, entity, boxColliderComp);

		if (SphereCollider* sphereColliderComp = ent.getComponent<SphereCollider>())
			saveSphereColliderComponent(doc, entity, sphereColliderComp);

		if (CapsuleCollider* capsuleColliderComp = ent.getComponent<CapsuleCollider>())
			saveCapsuleColliderComponent(doc, entity, capsuleColliderComp);

		entitiesNode->append_node(entity);
	}

	std::string xml_as_string;
	rapidxml::print(std::back_inserter(xml_as_string), doc);

	std::ofstream file_stored(editor->fileSystem.assetsPathExternal + "\\MyProject\\Database\\entity.xml");
	file_stored << doc;
	file_stored.close();
	doc.clear();
	return true;
}

bool SaveLoadSystem::loadEntities(Editor* editor) {

	std::ifstream file(editor->fileSystem.assetsPathExternal + "\\MyProject\\Database\\entity.xml");

	if (file.fail())
		return false;

	rapidxml::xml_document<> doc;
	rapidxml::xml_node<>* root_node = NULL;

	std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	buffer.push_back('\0');

	doc.parse<0>(&buffer[0]);

	root_node = doc.first_node("Entities");

	for (rapidxml::xml_node<>* entity_node = root_node->first_node("Entity"); entity_node; entity_node = entity_node->next_sibling()) {
		
		Entity ent(entity_node->first_attribute("Name")->value(), editor->scene.entities);
		editor->scene.entities.push_back(ent);

		SaveLoadSystem::loadTransformComponent(&editor->scene.entities.back(), entity_node);
		SaveLoadSystem::loadLightComponent(editor, &editor->scene.entities.back(), entity_node);
		SaveLoadSystem::loadMeshRendererComponent(editor, &editor->scene.entities.back(), entity_node);
		SaveLoadSystem::loadRigidbodyComponent(editor, &editor->scene.entities.back(), entity_node);
		SaveLoadSystem::loadMeshColliderComponent(&editor->scene.entities.back(), entity_node);
		SaveLoadSystem::loadBoxColliderComponent(&editor->scene.entities.back(), entity_node);
		SaveLoadSystem::loadSphereColliderComponent(&editor->scene.entities.back(), entity_node);
		SaveLoadSystem::loadCapsuleColliderComponent(&editor->scene.entities.back(), entity_node);
	}

	file.close();
	return true;
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
	rigidbodyNode->append_attribute(doc.allocate_attribute("Mass", doc.allocate_string(std::to_string(rigidbody->mass).c_str())));
	rigidbodyNode->append_attribute(doc.allocate_attribute("UseGravity", doc.allocate_string(std::to_string(rigidbody->useGravity ? 1 : 0).c_str())));
	entNode->append_node(rigidbodyNode);

	return true;
}

bool SaveLoadSystem::loadRigidbodyComponent(Editor* editor, Entity* ent, rapidxml::xml_node<>* entNode) {

	rapidxml::xml_node<>* rigidbodyNode = entNode->first_node("Rigidbody");

	if (rigidbodyNode == NULL)
		return false;

	Rigidbody* rigidbodyComp = ent->addComponent<Rigidbody>();
	rigidbodyComp->mass = atof(rigidbodyNode->first_attribute("Mass")->value());
	rigidbodyComp->useGravity = atoi(rigidbodyNode->first_attribute("UseGravity")->value()) == 1 ? true : false;

	return true;
}

bool SaveLoadSystem::saveMeshColliderComponent(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* entNode, MeshCollider* meshCollider) {

	rapidxml::xml_node<>* meshColliderNode = doc.allocate_node(rapidxml::node_element, "MeshCollider");
	meshColliderNode->append_attribute(doc.allocate_attribute("Convex", doc.allocate_string(std::to_string(meshCollider->convex ? 1 : 0).c_str())));
	meshColliderNode->append_attribute(doc.allocate_attribute("Trigger", doc.allocate_string(std::to_string(meshCollider->trigger ? 1 : 0).c_str())));
	entNode->append_node(meshColliderNode);

	return true;
}

bool SaveLoadSystem::loadMeshColliderComponent(Entity* ent, rapidxml::xml_node<>* entNode) {

	rapidxml::xml_node<>* meshColliderNode = entNode->first_node("MeshCollider");

	if (meshColliderNode == NULL)
		return false;

	MeshCollider* meshColliderComp = ent->addComponent<MeshCollider>();
	meshColliderComp->convex = atoi(meshColliderNode->first_attribute("Convex")->value()) == 1 ? true : false;
	meshColliderComp->trigger = atoi(meshColliderNode->first_attribute("Trigger")->value()) == 1 ? true : false;

	return true;
}

bool SaveLoadSystem::saveBoxColliderComponent(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* entNode, BoxCollider* boxCollider) {

	rapidxml::xml_node<>* boxColliderNode = doc.allocate_node(rapidxml::node_element, "BoxCollider");
	boxColliderNode->append_attribute(doc.allocate_attribute("Trigger", doc.allocate_string(std::to_string(boxCollider->trigger ? 1 : 0).c_str())));

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

	entNode->append_node(boxColliderNode);

	return true;
}

bool SaveLoadSystem::loadBoxColliderComponent(Entity* ent, rapidxml::xml_node<>* entNode) {

	rapidxml::xml_node<>* boxColliderNode = entNode->first_node("BoxCollider");

	if (boxColliderNode == NULL)
		return false;

	BoxCollider* boxColliderComp = ent->addComponent<BoxCollider>();

	boxColliderComp->trigger = atoi(boxColliderNode->first_attribute("Trigger")->value()) == 1 ? true : false;

	boxColliderComp->center.x = atof(boxColliderNode->first_node("Center")->first_attribute("X")->value());
	boxColliderComp->center.y = atof(boxColliderNode->first_node("Center")->first_attribute("Y")->value());
	boxColliderComp->center.z = atof(boxColliderNode->first_node("Center")->first_attribute("Z")->value());

	boxColliderComp->size.x = atof(boxColliderNode->first_node("Size")->first_attribute("X")->value());
	boxColliderComp->size.y = atof(boxColliderNode->first_node("Size")->first_attribute("Y")->value());
	boxColliderComp->size.z = atof(boxColliderNode->first_node("Size")->first_attribute("Z")->value());

	return true;
}

bool SaveLoadSystem::saveSphereColliderComponent(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* entNode, SphereCollider* sphereCollider) {

	rapidxml::xml_node<>* sphereColliderNode = doc.allocate_node(rapidxml::node_element, "SphereCollider");

	sphereColliderNode->append_attribute(doc.allocate_attribute("Trigger", doc.allocate_string(std::to_string(sphereCollider->trigger ? 1 : 0).c_str())));
	sphereColliderNode->append_attribute(doc.allocate_attribute("Radius", doc.allocate_string(std::to_string(sphereCollider->radius).c_str())));

	rapidxml::xml_node<>* centerNode = doc.allocate_node(rapidxml::node_element, "Center");
	centerNode->append_attribute(doc.allocate_attribute("X", doc.allocate_string(std::to_string(sphereCollider->center.x).c_str())));
	centerNode->append_attribute(doc.allocate_attribute("Y", doc.allocate_string(std::to_string(sphereCollider->center.y).c_str())));
	centerNode->append_attribute(doc.allocate_attribute("Z", doc.allocate_string(std::to_string(sphereCollider->center.z).c_str())));
	sphereColliderNode->append_node(centerNode);

	entNode->append_node(sphereColliderNode);

	return true;
}

bool SaveLoadSystem::loadSphereColliderComponent(Entity* ent, rapidxml::xml_node<>* entNode) {

	rapidxml::xml_node<>* sphereColliderNode = entNode->first_node("SphereCollider");

	if (sphereColliderNode == NULL)
		return false;

	SphereCollider* sphereColliderComp = ent->addComponent<SphereCollider>();

	sphereColliderComp->trigger = atoi(sphereColliderNode->first_attribute("Trigger")->value()) == 1 ? true : false;
	sphereColliderComp->radius = atof(sphereColliderNode->first_attribute("Radius")->value());

	sphereColliderComp->center.x = atof(sphereColliderNode->first_node("Center")->first_attribute("X")->value());
	sphereColliderComp->center.y = atof(sphereColliderNode->first_node("Center")->first_attribute("Y")->value());
	sphereColliderComp->center.z = atof(sphereColliderNode->first_node("Center")->first_attribute("Z")->value());

	return true;
}

bool SaveLoadSystem::saveCapsuleColliderComponent(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* entNode, CapsuleCollider* capsuleCollider) {

	rapidxml::xml_node<>* capsuleColliderNode = doc.allocate_node(rapidxml::node_element, "CapsuleCollider");

	capsuleColliderNode->append_attribute(doc.allocate_attribute("Trigger", doc.allocate_string(std::to_string(capsuleCollider->trigger ? 1 : 0).c_str())));
	capsuleColliderNode->append_attribute(doc.allocate_attribute("Radius", doc.allocate_string(std::to_string(capsuleCollider->radius).c_str())));
	capsuleColliderNode->append_attribute(doc.allocate_attribute("Height", doc.allocate_string(std::to_string(capsuleCollider->height).c_str())));
	capsuleColliderNode->append_attribute(doc.allocate_attribute("Axis", doc.allocate_string(std::to_string(capsuleCollider->axis).c_str())));

	rapidxml::xml_node<>* centerNode = doc.allocate_node(rapidxml::node_element, "Center");
	centerNode->append_attribute(doc.allocate_attribute("X", doc.allocate_string(std::to_string(capsuleCollider->center.x).c_str())));
	centerNode->append_attribute(doc.allocate_attribute("Y", doc.allocate_string(std::to_string(capsuleCollider->center.y).c_str())));
	centerNode->append_attribute(doc.allocate_attribute("Z", doc.allocate_string(std::to_string(capsuleCollider->center.z).c_str())));
	capsuleColliderNode->append_node(centerNode);

	entNode->append_node(capsuleColliderNode);

	return true;
}

bool SaveLoadSystem::loadCapsuleColliderComponent(Entity* ent, rapidxml::xml_node<>* entNode) {

	rapidxml::xml_node<>* capsuleColliderNode = entNode->first_node("CapsuleCollider");

	if (capsuleColliderNode == NULL)
		return false;

	CapsuleCollider* capsuleColliderComp = ent->addComponent<CapsuleCollider>();

	capsuleColliderComp->trigger = atoi(capsuleColliderNode->first_attribute("Trigger")->value()) == 1 ? true : false;
	capsuleColliderComp->radius = atof(capsuleColliderNode->first_attribute("Radius")->value());
	capsuleColliderComp->height = atof(capsuleColliderNode->first_attribute("Height")->value());
	capsuleColliderComp->axis = atoi(capsuleColliderNode->first_attribute("Axis")->value());

	capsuleColliderComp->center.x = atof(capsuleColliderNode->first_node("Center")->first_attribute("X")->value());
	capsuleColliderComp->center.y = atof(capsuleColliderNode->first_node("Center")->first_attribute("Y")->value());
	capsuleColliderComp->center.z = atof(capsuleColliderNode->first_node("Center")->first_attribute("Z")->value());

	return true;
}
