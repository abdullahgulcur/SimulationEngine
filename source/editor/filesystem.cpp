#include "filesystem.hpp"

FileSystem::FileSystem() {

}

FileSystem::~FileSystem() {

}

void FileSystem::initFileSystem() {

	FileSystem::checkProjectFolder();
	FileSystem::loadDefaultAssets();
	FileSystem::initEditorTextures();

	file = new File;
	file->id = files.size();
	file->parent = NULL;

	FileNode fileNode;

	fileNode.addr = file;
	fileNode.path = assetsPathExternal + "\\MyProject";
	fileNode.name = "MyProject";
	fileNode.extension = "";
	fileNode.type = FileType::folder;
	fileNode.textureID = editorTextures.folderBigTextureID;
	files.push_back(fileNode);

	FileSystem::generateFileStructure(file);
}

void FileSystem::checkProjectFolder() {

	PWSTR ppszPath; // variable to receive the path memory block pointer.

	HRESULT hr = SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &ppszPath);

	std::wstring documentsPath;
	if (SUCCEEDED(hr))
		documentsPath = ppszPath; // make a local copy of the path

	CoTaskMemFree(ppszPath);      // free up the path memory block

	const std::string temp(documentsPath.begin(), documentsPath.end());
	assetsPathExternal = temp + "\\Fury";

	if (!std::filesystem::exists(assetsPathExternal))
		std::filesystem::create_directory(assetsPathExternal);

	if (!std::filesystem::exists(assetsPathExternal + "\\MyProject"))
		std::filesystem::create_directory(assetsPathExternal + "\\MyProject");

	if (!std::filesystem::exists(assetsPathExternal + "\\MyProject\\Folders"))
		std::filesystem::create_directory(assetsPathExternal + "\\MyProject\\Folders");

	//if (!std::filesystem::exists(assetsPathExternal + "\\MyProject\\Database"))
	//	std::filesystem::create_directory(assetsPathExternal + "\\MyProject\\Database");
}

void FileSystem::initEditorTextures() {

	Texture texture;
	editorTextures.openFolderTextureID = texture.loadDDS("resource/icons/folder_open.DDS");
	editorTextures.closedFolderTextureID = texture.loadDDS("resource/icons/folder_closed.DDS");
	editorTextures.objectTextureID = texture.loadDDS("resource/icons/icon_object.DDS");
	editorTextures.objectBigTextureID = texture.loadDDS("resource/icons/object_big.DDS");
	editorTextures.textureTextureID = texture.loadDDS("resource/icons/icon_texture.DDS");
	editorTextures.undefinedTextureID = texture.loadDDS("resource/icons/undefined.DDS");
	editorTextures.documentTextureID = texture.loadDDS("resource/icons/icon_document.DDS");
	editorTextures.folderBigTextureID = texture.loadDDS("resource/icons/folder_closed_big.DDS");
	editorTextures.plusTextureID = texture.loadDDS("resource/icons/plus.DDS");
	editorTextures.materialTextureID = texture.loadDDS("resource/icons/material.DDS");
}

void FileSystem::generateFileStructure(File* file) {

	for (std::filesystem::path entry : std::filesystem::directory_iterator(files[file->id].path)) {

		File* subfile = new File;
		subfile->id = files.size();
		subfile->parent = file;

		FileNode fileNode;
		fileNode.path = entry.string();
		fileNode.name = entry.stem().string();
		fileNode.extension = entry.extension().string();
		fileNode.type = FileSystem::getFileType(entry.extension().string());
		fileNode.addr = subfile;
		loadFileToEngine(fileNode);
		files.push_back(fileNode);

		(file->subfiles).push_back(subfile);

		if(fileNode.type == FileType::folder)
			FileSystem::generateFileStructure(subfile);
	}
}

void FileSystem::updateChildrenPathRecursively(File* file) {

	for (int i = 0; i < file->subfiles.size(); i++) {

		std::stack<File*>fileStack;
		File* iter = file->subfiles[i];

		while (iter->parent != NULL) {

			fileStack.push(files[iter->id].addr);
			iter = iter->parent;
		}

		std::string updatedPath = assetsPathExternal + "\\MyProject\\";

		while (fileStack.size() > 1) {

			File* popped = fileStack.top();
			fileStack.pop();
			updatedPath = updatedPath + files[popped->id].name + "\\";
		}

		std::string oldPath = files[file->subfiles[i]->id].path;
		File* popped = fileStack.top();
		fileStack.pop();
		updatedPath = updatedPath + files[popped->id].name + files[popped->id].extension;
		files[file->subfiles[i]->id].path = updatedPath;

		FileSystem::changeAssetsKeyManually(file->subfiles[i]->id, oldPath, files[file->subfiles[i]->id].path);

		FileSystem::updateChildrenPathRecursively(file->subfiles[i]);
	}
}

void FileSystem::traverseAllFiles(File* file) {

	for (int i = 0; i < file->subfiles.size(); i++)
		traverseAllFiles(file->subfiles[i]);

	std::cout << files[file->id].path << std::endl;
}

void FileSystem::getTreeIndices(File* file, std::vector<int>& indices) {

	for (int i = 0; i < file->subfiles.size(); i++)
		getTreeIndices(file->subfiles[i], indices);

	indices.push_back(file->id);
}

bool FileSystem::subfolderAndItselfCheck(File* fileToMove, File* fileToBeMoved) {

	while (fileToMove->parent != NULL) {

		if (fileToMove == fileToBeMoved)
			return true;
		
		fileToMove = fileToMove->parent;
	}
	return false;
}

bool FileSystem::subfolderCheck(File* fileToMove, File* fileToBeMoved) {

	while (fileToMove->parent != NULL) {

		if (fileToMove->parent == fileToBeMoved)
			return true;

		fileToMove = fileToMove->parent;
	}
	return false;
}

void FileSystem::moveFile(int toBeMoved, int moveTo) {

	if (files[moveTo].type != FileType::folder || FileSystem::subfolderAndItselfCheck(files[moveTo].addr, files[toBeMoved].addr) || files[toBeMoved].addr->parent == files[moveTo].addr)
		return;

	std::string previousPath = files[toBeMoved].path;
	File* oldParent = files[toBeMoved].addr->parent;
	files[toBeMoved].addr->parent = files[moveTo].addr;

	const auto copyOptions = std::filesystem::copy_options::recursive;

	if (files[moveTo].addr->subfiles.size() == 0) {
		(files[moveTo].addr->subfiles).push_back(files[toBeMoved].addr);
		std::string destination = files[moveTo].path + "\\" + files[toBeMoved].name + files[toBeMoved].extension;
		files[toBeMoved].path = destination;
		FileSystem::updateChildrenPathRecursively(files[toBeMoved].addr);
		std::filesystem::copy(previousPath, destination, copyOptions);
		std::filesystem::remove_all(previousPath);

		FileSystem::changeAssetsKeyManually(toBeMoved, previousPath, destination);
	}
	else {

		files[toBeMoved].name = FileSystem::getAvailableFileName(files[toBeMoved].addr, files[toBeMoved].name.c_str());
		files[toBeMoved].path = files[files[toBeMoved].addr->parent->id].path +"\\" + files[toBeMoved].name + files[toBeMoved].extension;
		FileSystem::insertFileToParentsSubfolders(files[toBeMoved].addr);
		FileSystem::updateChildrenPathRecursively(files[toBeMoved].addr);

		std::filesystem::copy(previousPath, files[toBeMoved].path, copyOptions);
		std::filesystem::remove_all(previousPath);

		FileSystem::changeAssetsKeyManually(toBeMoved, previousPath, files[toBeMoved].path);
	}

	FileSystem::deleteFileFromTree(oldParent, toBeMoved);
}

void FileSystem::changeAssetsKeyManually(int toBeMoved, std::string previousPath, std::string destination) {

	switch (files[toBeMoved].type) {

	case FileType::material: {

		previousPath.erase(0, assetsPathExternal.length());
		destination.erase(0, assetsPathExternal.length());
		auto it = materials.extract(previousPath);
		it.key() = destination;
		materials.insert(std::move(it));
		break;
	}
	case FileType::object: {

		break;
	}
	case FileType::texture: {

		previousPath.erase(0, assetsPathExternal.length());
		destination.erase(0, assetsPathExternal.length());
		auto it = textures.extract(previousPath);
		it.key() = destination;
		textures.insert(std::move(it));
		break;
	}
	}
}

void FileSystem::deleteFileCompletely(int id) {

	std::filesystem::remove_all(files[id].path);

	std::vector<int> indices;
	FileSystem::getTreeIndices(files[id].addr, indices);
	std::sort(indices.begin(), indices.end(), std::greater<int>());

	File* parent = files[id].addr->parent;
	FileSystem::deleteFileFromTree(parent, id);

	for (int i = 0; i < indices.size(); i++) {

		switch (files[indices[i]].type) {

		case FileType::material: {
			std::string relativePath = files[indices[i]].path;
			relativePath.erase(0, assetsPathExternal.length());
			materials.erase(relativePath);
			// delete texture ids
			//std::unordered_map<std::string, Material>::const_iterator it = materials.find(relativePath);
			break;
		}
		case FileType::texture: {

			std::string relativePath = files[indices[i]].path;
			relativePath.erase(0, assetsPathExternal.length());
			textures.erase(relativePath);
			// delete texture ids
			//std::unordered_map<std::string, Texture>::const_iterator it = textures.find(relativePath);
			break;
		}
		}

		delete files[indices[i]].addr;
		files.erase(files.begin() + indices[i]);
	}

	for (int i = indices[indices.size() - 1]; i < files.size(); i++)
		files[i].addr->id = i;
}

void FileSystem::deleteFileFromTree(File* parent, int id) {

	for (int i = 0; i < parent->subfiles.size(); i++) {

		if (parent->subfiles[i]->id == id) {

			parent->subfiles.erase(parent->subfiles.begin() + i);
			break;
		}
	}
}

void FileSystem::newFolder(int currentDirID, const char* fileName) {

	File* subFile = new File;
	subFile->id = files.size();
	subFile->parent = files[currentDirID].addr;

	// called this temp becasuse its properties can be changed afterwards 
	FileNode tempFileNode;
	std::string temp = fileName;
	tempFileNode.path = files[currentDirID].path + "\\" + temp;
	tempFileNode.name = temp;
	tempFileNode.extension = "";
	tempFileNode.type = FileType::folder;
	tempFileNode.addr = subFile;
	files.push_back(tempFileNode);

	if (files[currentDirID].addr->subfiles.size() == 0) {
		(files[currentDirID].addr->subfiles).push_back(subFile);
		std::filesystem::create_directory(files[subFile->id].path);
	}
	else {

		files[subFile->id].name = FileSystem::getAvailableFileName(subFile, files[subFile->id].name.c_str());
		files[subFile->id].path = files[subFile->parent->id].path +"\\" + files[subFile->id].name + files[subFile->id].extension;
		FileSystem::insertFileToParentsSubfolders(subFile);

		std::filesystem::create_directory(files[subFile->id].path);
	}

	loadFileToEngine(files[subFile->id]);
}

void FileSystem::newMaterial(int currentDirID, const char* fileName) {

	File* subFile = new File;
	subFile->id = files.size();
	subFile->parent = files[currentDirID].addr;

	// called this temp becasuse its properties can be changed afterwards 
	FileNode tempFileNode;
	std::string temp = fileName;
	tempFileNode.path = files[currentDirID].path + "\\" + temp + ".mat";
	tempFileNode.name = temp;
	tempFileNode.extension = ".mat";
	tempFileNode.type = FileType::material;
	tempFileNode.addr = subFile;
	files.push_back(tempFileNode);

	if (files[currentDirID].addr->subfiles.size() == 0)
		(files[currentDirID].addr->subfiles).push_back(subFile);
	else {

		files[subFile->id].name = FileSystem::getAvailableFileName(subFile, files[subFile->id].name.c_str());
		files[subFile->id].path = files[subFile->parent->id].path + "\\" + files[subFile->id].name + files[subFile->id].extension;
		FileSystem::insertFileToParentsSubfolders(subFile);
	}

	Material mat;
	std::string relativePath = files[subFile->id].path;
	relativePath.erase(0, assetsPathExternal.length());
	materials.insert({ relativePath, mat });
	FileSystem::writeMaterialFile(files[subFile->id].path, mat);
	files[subFile->id].textureID = editorTextures.materialTextureID;
}

void FileSystem::readMaterialFile(std::string path, Material& mat) {

	std::ifstream file(path);

	rapidxml::xml_document<> doc;
	rapidxml::xml_node<>* root_node = NULL;

	std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	buffer.push_back('\0');

	doc.parse<0>(&buffer[0]);

	root_node = doc.first_node("Material");

	mat.type = std::strcmp("PBR", root_node->first_node("Type")->value()) == 0 ? MaterialType::pbr : MaterialType::phong;
	mat.albedoTexturePath = root_node->first_node("AlbedoMap")->value();
	mat.normalTexturePath = root_node->first_node("NormalMap")->value();
	mat.metallicTexturePath = root_node->first_node("MetallicMap")->value();
	mat.roughnessTexturePath = root_node->first_node("RoughnessMap")->value();
	mat.aoTexturePath = root_node->first_node("AOMAP")->value();

	mat.useAlbedo = std::strcmp(mat.albedoTexturePath.c_str(), "Null") != 0;
	mat.useNormal = std::strcmp(mat.normalTexturePath.c_str(), "Null") != 0;
	mat.useMetallic = std::strcmp(mat.metallicTexturePath.c_str(), "Null") != 0;
	mat.useRoughness = std::strcmp(mat.roughnessTexturePath.c_str(), "Null") != 0;
	mat.useAO = std::strcmp(mat.aoTexturePath.c_str(), "Null") != 0;

	mat.normalAmount = atof(root_node->first_node("Amount")->first_attribute("Normal")->value());
	mat.metallicAmount = atof(root_node->first_node("Amount")->first_attribute("Metallic")->value());
	mat.roughnessAmount = atof(root_node->first_node("Amount")->first_attribute("Roughness")->value());

	mat.albedoColor.x = atof(root_node->first_node("AlbedoColor")->first_attribute("X")->value());
	mat.albedoColor.y = atof(root_node->first_node("AlbedoColor")->first_attribute("Y")->value());
	mat.albedoColor.z = atof(root_node->first_node("AlbedoColor")->first_attribute("Z")->value());

}

void FileSystem::writeMaterialFile(std::string path, Material mat) {

	rapidxml::xml_document<> doc;
	rapidxml::xml_node<>* decl = doc.allocate_node(rapidxml::node_declaration);
	decl->append_attribute(doc.allocate_attribute("version", "1.0"));
	decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
	doc.append_node(decl);

	rapidxml::xml_node<>* materialNode = doc.allocate_node(rapidxml::node_element, "Material");

	const char* shaderType = mat.type == MaterialType::pbr ? "PBR" : "Phong";
	rapidxml::xml_node<>* typeNode = doc.allocate_node(rapidxml::node_element, "Type");
	typeNode->value(doc.allocate_string(shaderType));
	materialNode->append_node(typeNode);

	const char* nullStr = "Null";
	const char* albedoMapPath = mat.useAlbedo ? mat.albedoTexturePath.c_str() : nullStr;
	rapidxml::xml_node<>* albedoPathNode = doc.allocate_node(rapidxml::node_element, "AlbedoMap");
	albedoPathNode->value(doc.allocate_string(albedoMapPath));
	materialNode->append_node(albedoPathNode);

	const char* normalMapPath = mat.useNormal ? mat.normalTexturePath.c_str() : nullStr;
	rapidxml::xml_node<>* normalPathNode = doc.allocate_node(rapidxml::node_element, "NormalMap");
	normalPathNode->value(doc.allocate_string(normalMapPath));
	materialNode->append_node(normalPathNode);

	const char* metallicMapPath = mat.useMetallic ? mat.metallicTexturePath.c_str() : nullStr;
	rapidxml::xml_node<>* metallicPathNode = doc.allocate_node(rapidxml::node_element, "MetallicMap");
	metallicPathNode->value(doc.allocate_string(metallicMapPath));
	materialNode->append_node(metallicPathNode);

	const char* roughnessMapPath = mat.useRoughness ? mat.roughnessTexturePath.c_str() : nullStr;
	rapidxml::xml_node<>* roughnessPathNode = doc.allocate_node(rapidxml::node_element, "RoughnessMap");
	roughnessPathNode->value(doc.allocate_string(roughnessMapPath));
	materialNode->append_node(roughnessPathNode);

	const char* aoMapPath = mat.useAO ? mat.aoTexturePath.c_str() : nullStr;
	rapidxml::xml_node<>* aoPathNode = doc.allocate_node(rapidxml::node_element, "AOMAP");
	aoPathNode->value(doc.allocate_string(aoMapPath));
	materialNode->append_node(aoPathNode);

	rapidxml::xml_node<>* amountNode = doc.allocate_node(rapidxml::node_element, "Amount");
	amountNode->append_attribute(doc.allocate_attribute("Normal", doc.allocate_string(std::to_string(mat.normalAmount).c_str())));
	amountNode->append_attribute(doc.allocate_attribute("Metallic", doc.allocate_string(std::to_string(mat.metallicAmount).c_str())));
	amountNode->append_attribute(doc.allocate_attribute("Roughness", doc.allocate_string(std::to_string(mat.roughnessAmount).c_str())));
	materialNode->append_node(amountNode);

	rapidxml::xml_node<>* colorNode = doc.allocate_node(rapidxml::node_element, "AlbedoColor");
	colorNode->append_attribute(doc.allocate_attribute("X", doc.allocate_string(std::to_string(mat.albedoColor.x).c_str())));
	colorNode->append_attribute(doc.allocate_attribute("Y", doc.allocate_string(std::to_string(mat.albedoColor.y).c_str())));
	colorNode->append_attribute(doc.allocate_attribute("Z", doc.allocate_string(std::to_string(mat.albedoColor.z).c_str())));
	materialNode->append_node(colorNode);

	doc.append_node(materialNode);

	std::string xml_as_string;
	rapidxml::print(std::back_inserter(xml_as_string), doc);

	std::ofstream file_stored(path);
	file_stored << doc;
	file_stored.close();
	doc.clear();
}

void FileSystem::rename(int id, const char* newName) {

	if (Utility::iequals(newName, files[id].name) == 0)
		return;

	std::string oldPath = files[id].path;
	files[id].name = FileSystem::getAvailableFileName(files[id].addr, newName);
	files[id].path = files[files[id].addr->parent->id].path +"\\" + files[id].name + files[id].extension;
	files[id].addr->parent->subfiles.erase(files[id].addr->parent->subfiles.begin() + FileSystem::getSubFileIndex(files[id].addr));
	FileSystem::insertFileToParentsSubfolders(files[id].addr);
	std::filesystem::rename(oldPath, files[id].path);
	FileSystem::updateChildrenPathRecursively(files[id].addr);
	FileSystem::changeAssetsKeyManually(id, oldPath, files[id].path);
}

unsigned int FileSystem::getSubFileIndex(File* file) {

	for (int i = 0; i < file->parent->subfiles.size(); i++) {

		if (file->id == file->parent->subfiles[i]->id)
			return i;
	}
}

void FileSystem::insertFileToParentsSubfolders(File* file) {

	bool lastElementFlag = true;
	for (int i = 0; i < file->parent->subfiles.size(); i++) {

		if (Utility::iequals(files[file->id].name, files[file->parent->subfiles[i]->id].name) < 0) {

			file->parent->subfiles.insert(file->parent->subfiles.begin() + i, file);
			lastElementFlag = false;
			break;
		}
	}
	if (lastElementFlag)
		file->parent->subfiles.push_back(file);
}

std::string FileSystem::getAvailableFileName(File* file, const char* name) {

	std::vector<int> indices;
	bool flag = false;
	int fileSubIndex = -1;
	for (int i = 0; i < file->parent->subfiles.size(); i++) {

		if (Utility::iequals(name, files[file->parent->subfiles[i]->id].name) == 0)
			flag = true;

		std::string part = files[file->parent->subfiles[i]->id].name.substr(0, std::string(name).length());

		if (Utility::iequals(name, part) == 0) {

			indices.push_back(file->parent->subfiles[i]->id);
		}

		if (file->id == file->parent->subfiles[i]->id)
			fileSubIndex = i;
	}

	int max = 0;
	if (flag) {

		for (int i = 0; i < indices.size(); i++) {

			std::string temp = files[indices[i]].name.substr(0, std::string(name).length());
			const char* firstPart = temp.c_str();

			std::string rightPart = files[indices[i]].name.substr(std::string(name).length(), files[indices[i]].name.length() - std::string(name).length());
			const char* secPart = rightPart.c_str();

			if (!rightPart.empty() && std::all_of(rightPart.begin(), rightPart.end(), ::isdigit)) {
				if (stoi(rightPart) > max)
					max = stoi(rightPart);
			}
		}

		return std::string(name) + std::to_string(max + 1);
	}

	return std::string(name);
}

void FileSystem::duplicateFile(int id) {

	File* subFile = new File;
	subFile->id = files.size();
	subFile->parent = files[id].addr->parent;

	// called this temp becasuse its properties can be changed afterwards 
	FileNode tempFileNode = files[id];
	tempFileNode.addr = subFile;
	files.push_back(tempFileNode);

	files[subFile->id].name = FileSystem::getAvailableFileName(subFile, files[subFile->id].name.c_str());
	files[subFile->id].path = files[subFile->parent->id].path + "\\" + files[subFile->id].name + files[subFile->id].extension;
	FileSystem::insertFileToParentsSubfolders(subFile);
	FileSystem::updateChildrenPathRecursively(subFile);

	const auto copyOptions = std::filesystem::copy_options::recursive;
	std::filesystem::copy(files[id].path, files[subFile->id].path, copyOptions);

	FileSystem::loadFileToEngine(files[subFile->id]);

	if (files[subFile->id].type == FileType::folder)
		generateFileStructure(subFile);
}

FileType FileSystem::getFileType(std::string extension) {

	if (extension.empty())
		return FileType::folder;
	else if (extension == ".obj")
		return FileType::object;
	else if (extension == ".DDS")
		return FileType::texture;
	else if (extension == ".mat")
		return FileType::material;
	else
		return FileType::undefined;
}

bool FileSystem::hasSubFolder(File* file) {

	for (int i = 0; i < file->subfiles.size(); i++) {

		if (files[file->subfiles[i]->id].type == FileType::folder)
			return true;
	}
	return false;
}

void FileSystem::loadFileToEngine(FileNode& fileNode) {

	switch (fileNode.type)
	{
	case FileType::texture: {

		Texture texture;
		texture.setTextureID(fileNode.path.c_str());
		std::string relativePath = fileNode.path;
		relativePath.erase(0, assetsPathExternal.length());
		textures.insert({ relativePath, texture });
		fileNode.textureID = texture.textureID;
		break;
	}
	case FileType::object: {

		Model model;
		model.loadModel(fileNode.path.c_str());

		for (MeshRenderer renderer : model.meshes)
			meshes.push_back(renderer);

		fileNode.textureID = editorTextures.objectBigTextureID;
		break;
	}
	case FileType::folder: {

		fileNode.textureID = editorTextures.folderBigTextureID;
		break;
	}
	case FileType::material: {

		fileNode.textureID = editorTextures.materialTextureID;

		Material mat;
		readMaterialFile(fileNode.path, mat);
		std::string relativePath = fileNode.path;
		relativePath.erase(0, assetsPathExternal.length());
		materials.insert({ relativePath, mat });
		break;
	}
	case FileType::undefined: {

		fileNode.textureID = editorTextures.undefinedTextureID;
		break;
	}
	default:

		fileNode.textureID = editorTextures.undefinedTextureID;
		break;
	}
}

void FileSystem::importFiles(std::vector<std::string> filesToMove, int toDir) {

	for (int i = 0; i < filesToMove.size(); i++) {

		File* file = NULL;

		if (files[toDir].type != FileType::folder)
			file = files[toDir].addr->parent;
		else
			file = files[toDir].addr;

		File* subFile = new File;
		subFile->id = files.size();
		subFile->parent = file;

		// called this temp becasuse its properties can be changed afterwards 
		FileNode tempFileNode;
		std::filesystem::path fullPath = filesToMove[i];
		tempFileNode.name = fullPath.stem().string();
		tempFileNode.extension = fullPath.extension().string();
		tempFileNode.path = files[toDir].path + "\\" + tempFileNode.name + tempFileNode.extension;
		tempFileNode.type = FileSystem::getFileType(tempFileNode.extension);
		tempFileNode.addr = subFile;
		files.push_back(tempFileNode);

		const auto copyOptions = std::filesystem::copy_options::recursive;

		if (files[toDir].addr->subfiles.size() == 0) {

			(files[toDir].addr->subfiles).push_back(subFile);
			std::filesystem::copy(fullPath, files[subFile->id].path, copyOptions);
		}
		else {

			files[subFile->id].name = FileSystem::getAvailableFileName(subFile, files[subFile->id].name.c_str());
			files[subFile->id].path = files[files[subFile->id].addr->parent->id].path + +"\\" + files[subFile->id].name + files[subFile->id].extension;
			FileSystem::insertFileToParentsSubfolders(subFile);

			std::filesystem::copy(fullPath, files[subFile->id].path, copyOptions);
		}
		FileSystem::loadFileToEngine(files[subFile->id]);

		if (files[subFile->id].type == FileType::folder)
			generateFileStructure(subFile);
	}	
}

void FileSystem::loadDefaultAssets() {

	meshes.push_back(MeshRenderer());

	Texture texture;
	texture.setEmptyTextureID();
	textures.insert({"Null", texture });
}

//node* FileSystem::newNode(int data)
//{
//	node* Node = new node();
//	Node->data = data;
//	Node->left = NULL;
//	Node->right = NULL;
//
//	return(Node);
//}
//
//void FileSystem::identicalTrees(node* a, node* b, bool& identical)
//{
//	if (a == NULL && b == NULL)
//		return;
//	else if (a != NULL && b != NULL)
//	{
//		if (a->data != b->data) {
//
//			identical = false;
//			return;
//		}
//
//		identicalTrees(a->left, b->left, identical);
//		identicalTrees(a->right, b->right, identical);
//	}
//	else
//		identical = false;
//}
//
//// unfinished 
//void FileSystem::detectFilesChangedOutside(File* file, std::string fileToCompare)
//{
//	int count = 0;
//	for (std::filesystem::path entry : std::filesystem::directory_iterator(fileToCompare)) {
//
//		if (file->subfiles.size() == count) {
//
//			std::cout << "In folder: " << files[file->id].name << " and index: " << count << std::endl;
//			std::cout << "\nSHIT0!!!" << std::endl;
//			return;
//		}
//
//		if (std::strcmp(entry.string().c_str(), files[file->subfiles[count]->id].path.c_str()) != 0) {
//
//			std::cout << "In folder: " << files[file->id].name << " and index: " << count << std::endl;
//			std::cout << "\nSHIT1!!!" << std::endl;
//			return;
//		}
//
//		if (files[file->subfiles[count]->id].type == FileType::folder)
//			FileSystem::detectFilesChangedOutside(files[file->subfiles[count]->id].addr, entry.string());
//
//		count++;
//	}
//}