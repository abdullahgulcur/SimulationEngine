#include "filesystem.hpp"

FileSystem::FileSystem() {

}

void FileSystem::initFileSystem() {

	FileSystem::loadDefaultAssets();

	FileSystem::initEditorTextures();
	FileSystem::checkAssetsFolder();

	file = new File;
	file->id = index;
	file->parent = NULL;

	FileNode fileNode;

	fileNode.path = assetsPathExternal + "\\MyProject";
	fileNode.name = "MyProject";
	fileNode.extension = "";
	fileNode.type = FileType::folder;
	fileNode.textureID = editorTextures.folderBigTextureID;
	files[index] = fileNode;

	index++;

	FileSystem::generateFileStructure(file);
}

void FileSystem::checkAssetsFolder() {

	PWSTR ppszPath; // variable to receive the path memory block pointer.

	HRESULT hr = SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &ppszPath);

	std::wstring documentsPath;
	if (SUCCEEDED(hr))
		documentsPath = ppszPath; // make a local copy of the path

	CoTaskMemFree(ppszPath);      // free up the path memory block

	const std::string temp(documentsPath.begin(), documentsPath.end());
	assetsPathExternal = temp + "\\Fury";

	if (!std::filesystem::exists(assetsPathExternal)) {

		std::filesystem::create_directory(assetsPathExternal);
	}

	if (!std::filesystem::exists(assetsPathExternal + "\\MyProject")) {

		std::filesystem::create_directory(assetsPathExternal + "\\MyProject");
	}

	if (!std::filesystem::exists(assetsPathExternal + "\\MyProject\\Folders")) {

		std::filesystem::create_directory(assetsPathExternal + "\\MyProject\\Folders");
	}
}

void FileSystem::initEditorTextures() {

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
		subfile->id = index;
		subfile->parent = file;

		FileNode fileNode;
		fileNode.path = entry.string();
		fileNode.name = entry.stem().string();
		fileNode.extension = entry.extension().string();
		fileNode.type = FileSystem::getFileType(entry.extension().string());
		fileNode.addr = subfile;
		loadFileToEngine(fileNode);
		files[index] = fileNode;

		(file->subfiles).push_back(subfile);
		index++;

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

		std::string updatedPath = "MyProject\\";

		while (fileStack.size() > 1) {

			File* popped = fileStack.top();
			fileStack.pop();
			updatedPath = updatedPath + files[popped->id].name + "\\";
		}

		File* popped = fileStack.top();
		fileStack.pop();
		updatedPath = updatedPath + files[popped->id].name + files[popped->id].extension;

		files[file->subfiles[i]->id].path = updatedPath;

		FileSystem::updateChildrenPathRecursively(file->subfiles[i]);
	}

}

void FileSystem::traverseAllFiles(File* file) {

	for (int i = 0; i < file->subfiles.size(); i++)
		traverseAllFiles(file->subfiles[i]);

	std::cout << files[file->id].path << std::endl;
}

 /*
 * It checks if we try to move file to subfolders or file itself.
 * In this case function returns true.
 */
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

	if (files[moveTo].type != FileType::folder || FileSystem::subfolderAndItselfCheck(files[moveTo].addr, files[toBeMoved].addr))
		return;

	std::string previousPath = files[toBeMoved].path;
	File* oldParent = files[toBeMoved].addr->parent;
	files[toBeMoved].addr->parent = files[moveTo].addr;

	const auto copyOptions = std::filesystem::copy_options::recursive;

	if (files[moveTo].addr->subfiles.size() == 0) {
		(files[moveTo].addr->subfiles).push_back(files[toBeMoved].addr);
		std::string destination = files[moveTo].path + "\\" + files[toBeMoved].name + files[toBeMoved].extension;
		std::filesystem::copy(previousPath, destination, copyOptions);
		std::filesystem::remove_all(previousPath);
	}
	else {
		FileSystem::addFile(files[toBeMoved].addr, files[toBeMoved].name.c_str(), AddType::fromMove);
		std::filesystem::copy(previousPath, files[toBeMoved].path, copyOptions);
		std::filesystem::remove_all(previousPath);
	}

	FileSystem::deleteFileFromTree(oldParent, toBeMoved);
}

void FileSystem::deleteFileCompletely(int id) {

	std::filesystem::remove_all(files[id].path);

	File* parent = files[id].addr->parent;
	FileSystem::deleteFileFromTree(parent, id);
	files.erase(id);
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
	subFile->id = index;
	subFile->parent = files[currentDirID].addr;

	// called this temp becasuse its properties can be changed afterwards 
	FileNode tempFileNode;
	std::string temp = fileName;
	tempFileNode.path = files[currentDirID].path + "\\" + temp;
	tempFileNode.name = temp;
	tempFileNode.extension = "";
	tempFileNode.type = FileType::folder;
	tempFileNode.addr = subFile;
	files[index] = tempFileNode;
	index++;

	if (files[currentDirID].addr->subfiles.size() == 0) {
		(files[currentDirID].addr->subfiles).push_back(subFile);
		std::filesystem::create_directory(files[subFile->id].path);
	}
	else
		FileSystem::addFile(subFile, fileName, AddType::fromNewFolder);

	loadFileToEngine(files[subFile->id]);
}

void FileSystem::newMaterial(int currentDirID, const char* fileName) {

	File* subFile = new File;
	subFile->id = index;
	subFile->parent = files[currentDirID].addr;

	// called this temp becasuse its properties can be changed afterwards 
	FileNode tempFileNode;
	std::string temp = fileName;
	tempFileNode.path = files[currentDirID].path + "\\" + temp + ".mat";
	tempFileNode.name = temp;
	tempFileNode.extension = ".mat";
	tempFileNode.type = FileType::material;
	tempFileNode.addr = subFile;
	files[index] = tempFileNode;
	index++;

	(files[currentDirID].addr->subfiles).push_back(subFile);

	Material material;
	materials[subFile->id] = material;

	FileSystem::writeMaterial(material);

	loadFileToEngine(files[subFile->id]);
}

void FileSystem::writeMaterial(Material& material) {

	rapidxml::xml_document<> doc;
	rapidxml::xml_node<>* decl = doc.allocate_node(rapidxml::node_declaration);
	decl->append_attribute(doc.allocate_attribute("version", "1.0"));
	decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
	doc.append_node(decl);

	rapidxml::xml_node<>* materialNode = doc.allocate_node(rapidxml::node_element, "Material");

	const char* temp_val = doc.allocate_string("PBR");

	rapidxml::xml_node<>* typeNode = doc.allocate_node(rapidxml::node_element, "Type");
	typeNode->value(temp_val);
	materialNode->append_node(typeNode);

	temp_val = doc.allocate_string("Null");

	//if(material.useAlbedo)

	rapidxml::xml_node<>* albedoPathNode = doc.allocate_node(rapidxml::node_element, "AlbedoMap");
	albedoPathNode->value(temp_val);
	materialNode->append_node(albedoPathNode);

	rapidxml::xml_node<>* normalPathNode = doc.allocate_node(rapidxml::node_element, "NormalMap");
	normalPathNode->value(temp_val);
	materialNode->append_node(normalPathNode);

	rapidxml::xml_node<>* metallicPathNode = doc.allocate_node(rapidxml::node_element, "MetallicMap");
	metallicPathNode->value(temp_val);
	materialNode->append_node(metallicPathNode);

	rapidxml::xml_node<>* roughnessPathNode = doc.allocate_node(rapidxml::node_element, "RoughnessMap");
	roughnessPathNode->value(temp_val);
	materialNode->append_node(roughnessPathNode);

	rapidxml::xml_node<>* aoPathNode = doc.allocate_node(rapidxml::node_element, "AOMAP");
	aoPathNode->value(temp_val);
	materialNode->append_node(aoPathNode);

	temp_val = doc.allocate_string("False");
	rapidxml::xml_node<>* useMapNode = doc.allocate_node(rapidxml::node_element, "UseMap");
	useMapNode->append_attribute(doc.allocate_attribute("Albedo", temp_val));
	useMapNode->append_attribute(doc.allocate_attribute("Normal", temp_val));
	useMapNode->append_attribute(doc.allocate_attribute("Metallic", temp_val));
	useMapNode->append_attribute(doc.allocate_attribute("Roughness", temp_val));
	useMapNode->append_attribute(doc.allocate_attribute("AO", temp_val));
	materialNode->append_node(useMapNode);

	temp_val = doc.allocate_string("0.5");
	rapidxml::xml_node<>* amountNode = doc.allocate_node(rapidxml::node_element, "Amount");
	amountNode->append_attribute(doc.allocate_attribute("Normal", temp_val));
	amountNode->append_attribute(doc.allocate_attribute("Metallic", temp_val));
	materialNode->append_node(amountNode);

	temp_val = doc.allocate_string("1.0");
	rapidxml::xml_node<>* colorNode = doc.allocate_node(rapidxml::node_element, "AlbedoColor");
	colorNode->append_attribute(doc.allocate_attribute("X", temp_val));
	colorNode->append_attribute(doc.allocate_attribute("Y", temp_val));
	colorNode->append_attribute(doc.allocate_attribute("Z", temp_val));
	materialNode->append_node(colorNode);

	doc.append_node(materialNode);

	//std::string xml_as_string;
	//rapidxml::print(std::back_inserter(xml_as_string), doc);

	//std::string path = files[currentDirID].path + "\\" + fileName + ".mat";

	//std::ofstream file_stored(path);
	//file_stored << doc;
	//file_stored.close();
	//doc.clear();
}

void FileSystem::rename(int id, const char* newName) {

	if (Utility::iequals(newName, files[id].name) == 0)
		return;

	FileSystem::addFile(files[id].addr, newName, AddType::fromRename);
}

/*
* Returns 1 if there is a file with the same name
* Returns 2 if there is not a file with the same name
*/
void FileSystem::addFile(File* file, const char* name, AddType type) {

	std::string oldPath = files[file->id].path;
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
		files[file->id].name = std::string(name) + std::to_string(max + 1);
		files[file->id].path = files[file->parent->id].path + "\\" + std::string(name) + std::to_string(max + 1) + files[file->id].extension;

		if(type == AddType::fromRename)
			file->parent->subfiles.erase(file->parent->subfiles.begin() + fileSubIndex);

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

		if (type == AddType::fromNewFolder)
			std::filesystem::create_directory(files[file->id].path);
		else if (type == AddType::fromRename) {

			std::filesystem::rename(oldPath, files[file->id].path);
			FileSystem::updateChildrenPathRecursively(files[file->id].addr);
		}
		else if (type == AddType::fromMove || type == AddType::fromDuplicate) {

			FileSystem::updateChildrenPathRecursively(files[file->id].addr);
		}

		return;
	}

	/*
	* if there are not files with the same name
	*/
	files[file->id].name = std::string(name);
	files[file->id].path = files[file->parent->id].path + "\\" + std::string(name) + files[file->id].extension;

	if (type == AddType::fromRename)
		file->parent->subfiles.erase(file->parent->subfiles.begin() + fileSubIndex);

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

	if (type == AddType::fromNewFolder)
		std::filesystem::create_directory(files[file->id].path);
	else if (type == AddType::fromRename) {

		std::filesystem::rename(oldPath, files[file->id].path);
		FileSystem::updateChildrenPathRecursively(files[file->id].addr);
	}
	else if (type == AddType::fromMove || type == AddType::fromDuplicate) {

		FileSystem::updateChildrenPathRecursively(files[file->id].addr);
	}
}

void FileSystem::duplicateFile(int id) {

	File* subFile = new File;
	subFile->id = index;
	subFile->parent = files[id].addr->parent;

	// called this temp becasuse its properties can be changed afterwards 
	FileNode tempFileNode = files[id];
	tempFileNode.addr = subFile;
	files[index] = tempFileNode;
	index++;

	FileSystem::addFile(subFile, tempFileNode.name.c_str(), AddType::fromDuplicate);

	const auto copyOptions = std::filesystem::copy_options::recursive;
	std::filesystem::copy(files[id].path, files[subFile->id].path, copyOptions);

	loadFileToEngine(files[subFile->id]);

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

		unsigned int textureID = texture.loadDDS(fileNode.path.c_str());
		fileNode.textureID = textureID;
		textures.push_back(textureID);
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
		//Material material;
		//material.loadMaterial(fileNode.path.c_str());
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
		subFile->id = index;
		subFile->parent = file;

		// called this temp becasuse its properties can be changed afterwards 
		FileNode tempFileNode;
		std::filesystem::path fullPath = filesToMove[i];
		tempFileNode.name = fullPath.stem().string();
		tempFileNode.extension = fullPath.extension().string();
		tempFileNode.path = files[toDir].path + "\\" + tempFileNode.name + tempFileNode.extension;
		tempFileNode.type = FileSystem::getFileType(tempFileNode.extension);
		tempFileNode.addr = subFile;
		files[index] = tempFileNode;
		index++;

		const auto copyOptions = std::filesystem::copy_options::recursive;

		if (files[toDir].addr->subfiles.size() == 0) {

			(files[toDir].addr->subfiles).push_back(subFile);
			std::filesystem::copy(fullPath, files[subFile->id].path, copyOptions);
		}
		else {

			FileSystem::addFile(subFile, tempFileNode.name.c_str(), AddType::fromImport);
			std::filesystem::copy(fullPath, files[subFile->id].path, copyOptions);
		}
		loadFileToEngine(files[subFile->id]);

		if (files[subFile->id].type == FileType::folder)
			generateFileStructure(subFile);
	}	
}


node* FileSystem::newNode(int data)
{
	node* Node = new node();
	Node->data = data;
	Node->left = NULL;
	Node->right = NULL;

	return(Node);
}

void FileSystem::identicalTrees(node* a, node* b, bool& identical)
{
	if (a == NULL && b == NULL)
		return;
	else if (a != NULL && b != NULL)
	{
		if (a->data != b->data) {

			identical = false;
			return;
		}

		identicalTrees(a->left, b->left, identical);
		identicalTrees(a->right, b->right, identical);
	}
	else
		identical = false;
}

// unfinished moterfuker
void FileSystem::detectFilesChangedOutside(File* file, std::string fileToCompare)
{
	int count = 0;
	for (std::filesystem::path entry : std::filesystem::directory_iterator(fileToCompare)) {

		if (file->subfiles.size() == count) {

			std::cout << "In folder: " << files[file->id].name << " and index: " << count << std::endl;
			std::cout << "\nSHIT0!!!" << std::endl;
			return;
		}

		if (std::strcmp(entry.string().c_str(), files[file->subfiles[count]->id].path.c_str()) != 0) {

			std::cout << "In folder: " << files[file->id].name << " and index: " << count << std::endl;
			std::cout << "\nSHIT1!!!" << std::endl;
			return;
		}

		if (files[file->subfiles[count]->id].type == FileType::folder)
			FileSystem::detectFilesChangedOutside(files[file->subfiles[count]->id].addr, entry.string());

		count++;
	}
}

void FileSystem::loadDefaultAssets() {

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::string name = "Null";
	MeshRenderer nullMesh(vertices, indices, name);
	meshes.push_back(nullMesh);

	Texture texture;
	unsigned int emptyTextureID = texture.getEmptyTexture();
	textures.push_back(emptyTextureID);
}