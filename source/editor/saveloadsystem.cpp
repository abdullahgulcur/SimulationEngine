#include "saveloadsystem.hpp"
#include "editor.hpp"
#include "camera.hpp"

bool SaveLoadSystem::loadSceneGraph(Editor* editor) {

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

		editor->scene.initialSceneGraph[index] = numbers;
	}

	file.close();
	return true;
}

bool SaveLoadSystem::saveSceneGraph(Editor* editor) {

	std::ofstream file(editor->fileSystem.assetsPathExternal + "\\MyProject\\Database\\scenegraph_db.txt");

	if (file.fail())
		return false;

	std::ostringstream fileTextStream;

	std::queue<Transform*> entQueue;
	entQueue.push(editor->scene.rootTransform);
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
		entitiesNode->append_node(entity);
	}

	std::string xml_as_string;
	rapidxml::print(std::back_inserter(xml_as_string), doc);

	std::ofstream file_stored(editor->fileSystem.assetsPathExternal + "\\MyProject\\Database\\entity_db.xml");
	file_stored << doc;
	file_stored.close();
	doc.clear();
	return true;
}

bool SaveLoadSystem::loadEntities(Editor* editor) {

	std::ifstream file(editor->fileSystem.assetsPathExternal + "\\MyProject\\Database\\entity_db.xml");

	if (file.fail())
		return false;

	rapidxml::xml_document<> doc;
	rapidxml::xml_node<>* root_node = NULL;

	std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	buffer.push_back('\0');

	doc.parse<0>(&buffer[0]);

	root_node = doc.first_node("Entities");

	for (rapidxml::xml_node<>* entity_node = root_node->first_node("Entity"); entity_node; entity_node = entity_node->next_sibling())
		Entity ent(entity_node->first_attribute("Name")->value(), editor->scene.entities);

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

bool SaveLoadSystem::saveTransforms(Editor* editor) {

	rapidxml::xml_document<> doc;
	rapidxml::xml_node<>* decl = doc.allocate_node(rapidxml::node_declaration);
	decl->append_attribute(doc.allocate_attribute("version", "1.0"));
	decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
	doc.append_node(decl);

	rapidxml::xml_node<>* transformsNode = doc.allocate_node(rapidxml::node_element, "Transforms");
	doc.append_node(transformsNode);
	for (Entity& ent : editor->scene.entities) {

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
	return true;
}

bool SaveLoadSystem::loadTransforms(Editor* editor) {

	std::ifstream file(editor->fileSystem.assetsPathExternal + "\\MyProject\\Database\\transform_db.xml");

	if (file.fail())
		return false;

	rapidxml::xml_document<> doc;
	rapidxml::xml_node<>* root_node = NULL;

	std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	buffer.push_back('\0');

	doc.parse<0>(&buffer[0]);

	root_node = doc.first_node("Transforms");

	int count = 0;
	for (rapidxml::xml_node<>* transform_node = root_node->first_node("Transform"); transform_node; transform_node = transform_node->next_sibling()) {

		editor->scene.entities[count].transform->localPosition.x = atof(transform_node->first_node("Position")->first_attribute("X")->value());
		editor->scene.entities[count].transform->localPosition.y = atof(transform_node->first_node("Position")->first_attribute("Y")->value());
		editor->scene.entities[count].transform->localPosition.z = atof(transform_node->first_node("Position")->first_attribute("Z")->value());

		editor->scene.entities[count].transform->localRotation.x = atof(transform_node->first_node("Rotation")->first_attribute("X")->value());
		editor->scene.entities[count].transform->localRotation.y = atof(transform_node->first_node("Rotation")->first_attribute("Y")->value());
		editor->scene.entities[count].transform->localRotation.z = atof(transform_node->first_node("Rotation")->first_attribute("Z")->value());

		editor->scene.entities[count].transform->localScale.x = atof(transform_node->first_node("Scale")->first_attribute("X")->value());
		editor->scene.entities[count].transform->localScale.y = atof(transform_node->first_node("Scale")->first_attribute("Y")->value());
		editor->scene.entities[count].transform->localScale.z = atof(transform_node->first_node("Scale")->first_attribute("Z")->value());

		count++;
	}

	file.close();
	editor->scene.rootTransform->updateSelfAndChild();
	return true;
}

bool SaveLoadSystem::saveLights(Editor* editor) {

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
		lightNode->append_attribute(doc.allocate_attribute("Type", doc.allocate_string(editor->scene.getLightType(editor->scene.lightComponents[editor->scene.entities[it.entID].lightComponentIndex].type).c_str())));
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
	return true;
}

bool SaveLoadSystem::loadLights(Editor* editor) {

	std::ifstream file(editor->fileSystem.assetsPathExternal + "\\MyProject\\Database\\light_db.xml");

	if (file.fail())
		return false;

	rapidxml::xml_document<> doc;
	rapidxml::xml_node<>* root_node = NULL;

	std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	buffer.push_back('\0');

	doc.parse<0>(&buffer[0]);

	root_node = doc.first_node("Lights");

	for (rapidxml::xml_node<>* light_node = root_node->first_node("Light"); light_node; light_node = light_node->next_sibling()) {

		Light lightComp;
		lightComp.entID = atoi(light_node->first_attribute("EntID")->value());

		if (strcmp(light_node->first_attribute("Type")->value(), "Point") == 0) {

			editor->scene.pointLightCount++;
			lightComp.type = LightType::PointLight;
		}
		else {

			editor->scene.dirLightCount++;
			lightComp.type = LightType::DirectionalLight;
		}

		lightComp.power = atof(light_node->first_attribute("Power")->value());

		lightComp.color.x = atof(light_node->first_node("Color")->first_attribute("X")->value());
		lightComp.color.y = atof(light_node->first_node("Color")->first_attribute("Y")->value());
		lightComp.color.z = atof(light_node->first_node("Color")->first_attribute("Z")->value());

		editor->scene.lightComponents.push_back(lightComp);
		editor->scene.entities[lightComp.entID].lightComponentIndex = editor->scene.lightComponents.size() - 1;
	}

	file.close();
	editor->scene.recompileAllMaterials();
	return true;
}

bool SaveLoadSystem::saveMeshRenderers(Editor* editor) {

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
	return true;
}

bool SaveLoadSystem::loadMeshRenderers(Editor* editor) {

	std::ifstream file(editor->fileSystem.assetsPathExternal + "\\MyProject\\Database\\meshrenderer_db.xml");

	if (file.fail())
		return false;

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

		editor->scene.meshRendererComponents.push_back(m_rendererComp);
		editor->scene.entities[m_rendererComp.entID].m_rendererComponentIndex = editor->scene.meshRendererComponents.size() - 1;
	}

	file.close();
	return true;
}