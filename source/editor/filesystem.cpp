#include "editor.hpp"
#include "filesystem.hpp"

FileSystem::FileSystem() {

}

FileSystem::~FileSystem() {

}

void FileSystem::init(Editor* editor) {

	FileSystem::setEditor(editor);

	FileSystem::checkProjectFolder();
	FileSystem::loadDefaultAssets();
	FileSystem::initEditorTextures();

	rootFile = new File;
	rootFile->id = files.size();
	rootFile->parent = NULL;

	FileNode fileNode;

	fileNode.addr = rootFile;
	fileNode.path = assetsPathExternal + "\\MyProject";
	fileNode.name = "MyProject";
	fileNode.extension = "";
	fileNode.type = FileType::folder;
	fileNode.textureID = editorTextures.folderBigTextureID;
	files.push_back(fileNode);

	FileSystem::generateFileStructure(rootFile);
	FileSystem::loadFilesToEngine();
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

	if (!std::filesystem::exists(assetsPathExternal + "\\MyProject\\Database"))
		std::filesystem::create_directory(assetsPathExternal + "\\MyProject\\Database");
}

void FileSystem::initEditorTextures() {

	editorTextures.openFolderTextureID = TextureNS::loadDDS("resource/icons/folder_open.DDS");
	editorTextures.closedFolderTextureID = TextureNS::loadDDS("resource/icons/folder_closed.DDS");
	editorTextures.objectTextureID = TextureNS::loadDDS("resource/icons/icon_object.DDS");
	editorTextures.objectBigTextureID = TextureNS::loadDDS("resource/icons/object_big.DDS");
	editorTextures.textureTextureID = TextureNS::loadDDS("resource/icons/icon_texture.DDS");
	editorTextures.undefinedTextureID = TextureNS::loadDDS("resource/icons/undefined.DDS");
	editorTextures.documentTextureID = TextureNS::loadDDS("resource/icons/icon_document.DDS");
	editorTextures.folderBigTextureID = TextureNS::loadDDS("resource/icons/folder_closed_big.DDS");
	editorTextures.plusTextureID = TextureNS::loadDDS("resource/icons/plus.DDS");
	editorTextures.materialTextureID = TextureNS::loadDDS("resource/icons/material.DDS");
}

void FileSystem::generateFileStructure(File* file) {

	for (std::filesystem::path entry : std::filesystem::directory_iterator(files[file->id].path)) {

		FileNode fileNode;
		fileNode.path = entry.string();
		fileNode.name = entry.stem().string();

		if (std::strcmp(fileNode.name.c_str(), "Database") == 0)
			continue;

		File* subfile = new File;
		subfile->id = files.size();
		subfile->parent = file;

		fileNode.extension = entry.extension().string();
		fileNode.type = FileSystem::getFileType(entry.extension().string());
		fileNode.addr = subfile;
		files.push_back(fileNode);

		(file->subfiles).push_back(subfile);

		if(fileNode.type == FileType::folder)
			FileSystem::generateFileStructure(subfile);
	}
}

void FileSystem::loadFilesToEngine() {

	std::vector<int> vertfileIDs;
	std::vector<int> fragfileIDs;
	std::vector<int> texturefileIDs;
	std::vector<int> matfileIDs;
	std::vector<int> objfileIDs;

	for (int i = 1; i < files.size(); i++) {

		switch (files[i].type) {
		case FileType::fragshader:
			fragfileIDs.push_back(i);
			break;
		case FileType::material:
			matfileIDs.push_back(i);
			break;
		case FileType::object:
			objfileIDs.push_back(i);
			break;
		case FileType::texture:
			texturefileIDs.push_back(i);
			break;
		case FileType::vertshader:
			vertfileIDs.push_back(i);
			break;
		default:
			loadFileToEngine(files[i]);
		}
	}

	for (int& i : vertfileIDs)
		loadFileToEngine(files[i]);

	for (int& i : fragfileIDs)
		loadFileToEngine(files[i]);

	for (int& i : texturefileIDs)
		loadFileToEngine(files[i]);

	for (int& i : matfileIDs) {
		loadFileToEngine(files[i]);

		int count = 0;
		MaterialFile& mat = materials[files[i].path];
		for (auto& texFileAddr : mat.textureUnitFileAddrs) {

			auto found = textures.find(FileSystem::getTextureFilePath(texFileAddr));

			if (found != textures.end())
				mat.textureUnits.push_back(found->second.textureID);
			else {
				mat.textureUnits.push_back(textures["whitetexture"].textureID);
				mat.textureUnitFileAddrs[count] = NULL;
				FileSystem::writeMaterialFile(files[mat.fileAddr->id].path, mat);

				// ASSERT
				char logMsg[256];
				sprintf(logMsg, "File %s is not found. Blank texture is set as default.", files[texFileAddr->id].path.c_str());
				printf("%s", logMsg);
				//Log::assertMessage(logMsg, &editor->editorGUI);
			}
			count++;
		}
	}

	for (int& i : objfileIDs)
		loadFileToEngine(files[i]);
}

void FileSystem::loadFilesToEngine(std::vector<int>& indices) {

	std::vector<int> vertfileIDs;
	std::vector<int> fragfileIDs;
	std::vector<int> texturefileIDs;
	std::vector<int> matfileIDs;
	std::vector<int> objfileIDs;

	for (int i = 0; i < indices.size(); i++) {

		switch (files[indices[i]].type) {
		case FileType::fragshader:
			fragfileIDs.push_back(indices[i]);
			break;
		case FileType::material:
			matfileIDs.push_back(indices[i]);
			break;
		case FileType::object:
			objfileIDs.push_back(indices[i]);
			break;
		case FileType::texture:
			texturefileIDs.push_back(indices[i]);
			break;
		case FileType::vertshader:
			vertfileIDs.push_back(indices[i]);
			break;
		default:
			loadFileToEngine(files[indices[i]]);
		}
	}

	for(int& i: vertfileIDs)
		loadFileToEngine(files[i]);

	for (int& i : fragfileIDs)
		loadFileToEngine(files[i]);

	for (int& i : texturefileIDs)
		loadFileToEngine(files[i]);

	for (int& i : matfileIDs) {
		loadFileToEngine(files[i]);

		int count = 0;
		MaterialFile& mat = materials[files[i].path];
		for (auto& texFileAddr : mat.textureUnitFileAddrs) {

			auto found = textures.find(FileSystem::getTextureFilePath(texFileAddr));

			if (found != textures.end())
				mat.textureUnits.push_back(found->second.textureID);
			else {
				mat.textureUnits.push_back(textures["whitetexture"].textureID);
				mat.textureUnitFileAddrs[count] = NULL;
				FileSystem::writeMaterialFile(files[mat.fileAddr->id].path, mat);

				// ASSERT
				char logMsg[256];
				sprintf(logMsg, "File %s is not found. Blank texture is set as default.", files[texFileAddr->id].path.c_str());
				printf("%s", logMsg);
				//Log::assertMessage(logMsg, &editor->editorGUI);
			}
			count++;
		}
	}

	for (int& i : objfileIDs)
		loadFileToEngine(files[i]);
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

void FileSystem::changeAssetsKeyManually(int fileID, std::string previousPath, std::string newPath) {

	switch (files[fileID].type) {

	case FileType::object: {

		std::vector<MeshRenderer*> addrs = meshes[previousPath].meshRendererCompAddrs;
		MeshFile mesh = meshes[previousPath];
		auto it = meshes.extract(previousPath);
		it.key() = newPath;
		meshes[it.key()] = mesh;
		meshes.insert(std::move(it));

		for (auto& mesh_it : addrs)
			mesh_it->mesh = &meshes[newPath];

		editor->scene.saveEditorProperties();

		break;
	}
	case FileType::material: {

		std::vector<MeshRenderer*> addrs = materials[previousPath].meshRendererCompAddrs;
		MaterialFile mat = materials[previousPath];
		auto it = materials.extract(previousPath);
		it.key() = newPath;
		materials[it.key()] = mat;
		materials.insert(std::move(it));

		for (auto& mat_it : addrs)
			mat_it->mat = &materials[newPath];

		editor->scene.saveEditorProperties();

		break;
	}
	case FileType::vertshader: {

		for (auto& it : materials) {

			if (it.second.vertShaderFileAddr == files[fileID].addr)
				FileSystem::writeMaterialFile(files[it.second.fileAddr->id].path, it.second);
		}

		auto it = vertShaders.extract(previousPath);
		it.key() = newPath;
		vertShaders.insert(std::move(it));

		break;
	}
	case FileType::fragshader: {

		for (auto& it : materials) {

			if (it.second.fragShaderFileAddr == files[fileID].addr)
				FileSystem::writeMaterialFile(files[it.second.fileAddr->id].path, it.second);
		}

		auto it = fragShaders.extract(previousPath);
		it.key() = newPath;
		fragShaders.insert(std::move(it));

		break;
	}
	case FileType::texture: {

		for (auto& mat_iter : materials) {

			for (auto& texFileAddr : mat_iter.second.textureUnitFileAddrs) {

				if (texFileAddr == files[fileID].addr)
					FileSystem::writeMaterialFile(files[mat_iter.second.fileAddr->id].path, mat_iter.second);
			}
		}

		auto it = textures.extract(previousPath);
		it.key() = newPath;
		textures.insert(std::move(it));

		break;
	}
	}
}

void FileSystem::deleteFileCompletely(int id) {

	std::vector<int> indices;
	FileSystem::getTreeIndices(files[id].addr, indices);
	std::sort(indices.begin(), indices.end(), std::greater<int>());

	File* parent = files[id].addr->parent;
	FileSystem::deleteFileFromTree(parent, id);

	for (int i = 0; i < indices.size(); i++) {

		switch (files[indices[i]].type) {

		case FileType::object: {

			std::vector<MeshRenderer*> addrs = meshes[files[indices[i]].path].meshRendererCompAddrs;
			meshes.erase(files[indices[i]].path);

			for (auto& it : addrs)
				it->mesh = &meshes["Null"];

			editor->scene.saveEditorProperties();

			break;
		}
		case FileType::material: {

			std::vector<MeshRenderer*> addrs = materials[files[indices[i]].path].meshRendererCompAddrs;
			materials.erase(files[indices[i]].path);

			for (auto& it : addrs)
				it->mat = &materials["Default"];

			editor->scene.saveEditorProperties();

			break;
		}
		case FileType::texture: {

			textures[files[indices[i]].path].deleteTexture();
			textures.erase(files[indices[i]].path);

			for (auto& mat_iter : materials) {

				int count = 0;
				for (auto& texFileAddr : mat_iter.second.textureUnitFileAddrs) {

					if (texFileAddr == files[indices[i]].addr) {
						texFileAddr = NULL;
						mat_iter.second.textureUnits[count] = textures["whitetexture"].textureID;
						FileSystem::writeMaterialFile(files[mat_iter.second.fileAddr->id].path, mat_iter.second);
					}
					count++;
				}
			}

			break;
		}
		case FileType::vertshader: {

			for (auto& mat_iter : materials) {

				if (mat_iter.second.vertShaderFileAddr == files[indices[i]].addr){
					mat_iter.second.vertShaderFileAddr = NULL;
					FileSystem::writeMaterialFile(files[mat_iter.second.fileAddr->id].path, mat_iter.second);
					
					mat_iter.second.deleteProgram();
					mat_iter.second.compileShaders("source/shader/Default.vert",
						FileSystem::getFragShaderPath(mat_iter.second.fragShaderFileAddr),
						editor->scene.dirLightTransforms.size(), editor->scene.pointLightTransforms.size());
				}
			}

			vertShaders.erase(files[indices[i]].path);

			break;
		}
		case FileType::fragshader: {

			for (auto& mat_iter : materials) {

				if (mat_iter.second.fragShaderFileAddr == files[indices[i]].addr) {
					mat_iter.second.fragShaderFileAddr = NULL;
					mat_iter.second.textureUnitFileAddrs.clear();
					mat_iter.second.textureUnits.clear();
					mat_iter.second.floatUnits.clear();
					FileSystem::writeMaterialFile(files[mat_iter.second.fileAddr->id].path, mat_iter.second);
					
					mat_iter.second.deleteProgram();
					mat_iter.second.compileShaders(FileSystem::getVertShaderPath(mat_iter.second.vertShaderFileAddr),
						"source/shader/Default.frag",
						editor->scene.dirLightTransforms.size(), editor->scene.pointLightTransforms.size());
				}
			}

			fragShaders.erase(files[indices[i]].path);

			break;
		}
		}
	}

	for (int i = 0; i < indices.size(); i++) {

		std::filesystem::remove(files[indices[i]].path);
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

int FileSystem::newFolder(int currentDirID, const char* fileName) {

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

	return subFile->id;
}

void FileSystem::newMaterial(int currentDirID, const char* fileName) {

	File* subFile = new File;
	subFile->id = files.size();
	subFile->parent = files[currentDirID].addr;

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

	MaterialFile mat(subFile, NULL, NULL, "source/shader/Default.vert", "source/shader/Default.frag",
		editor->scene.dirLightTransforms.size(), editor->scene.pointLightTransforms.size());
	materials.insert({ files[subFile->id].path, mat });
	FileSystem::writeMaterialFile(files[subFile->id].path, mat);
	files[subFile->id].textureID = editorTextures.materialTextureID;
}

void FileSystem::readMaterialFile(File* filePtr, std::string path) {

	std::ifstream file(path);

	rapidxml::xml_document<> doc;
	rapidxml::xml_node<>* root_node = NULL;

	std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	buffer.push_back('\0');

	doc.parse<0>(&buffer[0]);

	root_node = doc.first_node("Material");

	bool fileMayCorrupted = false;

	const char* vertFilePath = root_node->first_node("Shader")->first_attribute("Vert")->value();
	const char* fragFilePath = root_node->first_node("Shader")->first_attribute("Frag")->value();
	File* vertFileAddr = FileSystem::getVertShaderAddr(vertFilePath);
	File* fragFileAddr = FileSystem::getFragShaderAddr(fragFilePath);

	if (vertFileAddr == NULL) {

		vertFilePath = "source/shader/Default.vert";
		fileMayCorrupted = true;
	}

	if (fragFileAddr == NULL) {

		fragFilePath = "source/shader/Default.frag";
		fileMayCorrupted = true;
	}

	MaterialFile mat(filePtr, vertFileAddr, fragFileAddr, vertFilePath, fragFilePath, editor->scene.dirLightTransforms.size(), editor->scene.pointLightTransforms.size());

	if (fragFileAddr != NULL) {

		for (rapidxml::xml_node<>* texpath_node = root_node->first_node("Sampler2Ds")->first_node("Texture"); texpath_node; texpath_node = texpath_node->next_sibling()) {

			File* textFile = FileSystem::getTextureFileAddr(texpath_node->first_attribute("Path")->value());
			mat.textureUnitFileAddrs.push_back(textFile);

			if (textFile != NULL)
				mat.textureUnits.push_back(textures[texpath_node->first_attribute("Path")->value()].textureID);
			else {

				mat.textureUnits.push_back(textures["whitetexture"].textureID);
				fileMayCorrupted = true;
			}
		}

		for (rapidxml::xml_node<>* texpath_node = root_node->first_node("Floats")->first_node("Float"); texpath_node; texpath_node = texpath_node->next_sibling())
			mat.floatUnits.push_back(atof(texpath_node->first_attribute("Value")->value()));
	}

	materials.insert({ path, mat });

	if (fileMayCorrupted)
		FileSystem::writeMaterialFile(path, mat);
}

void FileSystem::writeMaterialFile(std::string path, MaterialFile& mat) {

	rapidxml::xml_document<> doc;
	rapidxml::xml_node<>* decl = doc.allocate_node(rapidxml::node_declaration);
	decl->append_attribute(doc.allocate_attribute("version", "1.0"));
	decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
	doc.append_node(decl);

	rapidxml::xml_node<>* materialNode = doc.allocate_node(rapidxml::node_element, "Material");

	rapidxml::xml_node<>* shaderNode = doc.allocate_node(rapidxml::node_element, "Shader");
	shaderNode->append_attribute(doc.allocate_attribute("Vert", doc.allocate_string(FileSystem::getVertShaderPath(mat.vertShaderFileAddr))));
	shaderNode->append_attribute(doc.allocate_attribute("Frag", doc.allocate_string(FileSystem::getFragShaderPath(mat.fragShaderFileAddr))));
	materialNode->append_node(shaderNode);

	rapidxml::xml_node<>* sampler2DsNode = doc.allocate_node(rapidxml::node_element, "Sampler2Ds");
	for (auto& texUnitFileAddr : mat.textureUnitFileAddrs) {

		rapidxml::xml_node<>* texture = doc.allocate_node(rapidxml::node_element, "Texture");
		texture->append_attribute(doc.allocate_attribute("Path", doc.allocate_string(FileSystem::getTextureFilePath(texUnitFileAddr))));
		sampler2DsNode->append_node(texture);
	}
	materialNode->append_node(sampler2DsNode);

	rapidxml::xml_node<>* floatsNode = doc.allocate_node(rapidxml::node_element, "Floats");
	for (float val : mat.floatUnits) {

		rapidxml::xml_node<>* floatNode = doc.allocate_node(rapidxml::node_element, "Float");
		floatNode->append_attribute(doc.allocate_attribute("Value", doc.allocate_string(std::to_string(val).c_str())));
		floatsNode->append_node(floatNode);
	}
	materialNode->append_node(floatsNode);

	doc.append_node(materialNode);

	std::string xml_as_string;
	rapidxml::print(std::back_inserter(xml_as_string), doc);

	std::ofstream file_stored(path);
	file_stored << doc;
	file_stored.close();
	doc.clear();
}

File* FileSystem::getTextureFileAddr(const char* path) {

	if (strcmp(path, "whitetexture") == 0)
		return NULL;

	if (textures.find(path) == textures.end())
		return NULL;
	
	return textures[path].fileAddr;
}

const char* FileSystem::getTextureFilePath(File* addr) {

	if (addr == NULL)
		return "whitetexture";

	return files[addr->id].path.c_str();
}

File* FileSystem::getFragShaderAddr(const char* path) {

	if (strcmp(path, "source/shader/Default.frag") == 0)
		return NULL;

	if (fragShaders.find(path) == fragShaders.end())
		return NULL;

	return fragShaders[path].fileAddr;
}

File* FileSystem::getVertShaderAddr(const char* path) {

	if (strcmp(path, "source/shader/Default.vert") == 0)
		return NULL;

	if (vertShaders.find(path) == vertShaders.end())
		return NULL;

	return vertShaders[path].fileAddr;
}

const char* FileSystem::getFragShaderPath(File* fileAddr) {

	if (fileAddr == NULL)
		return "source/shader/Default.frag";

	for (auto& fs : fragShaders) {

		if (fs.second.fileAddr == fileAddr)
			return files[fs.second.fileAddr->id].path.c_str();
	}
}

const char* FileSystem::getVertShaderPath(File* fileAddr) {

	if (fileAddr == NULL)
		return "source/shader/Default.vert";

	for (auto& vs : vertShaders) {

		if (vs.second.fileAddr == fileAddr)
			return files[vs.second.fileAddr->id].path.c_str();
	}
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

	if (files[id].type == FileType::material) {
		writeMaterialFile(files[id].path, materials[files[id].path]);
	}
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

int FileSystem::duplicateFile(int id) {

	File* subFile = new File;
	subFile->id = files.size();
	subFile->parent = files[id].addr->parent;

	FileNode tempFileNode = files[id];
	tempFileNode.addr = subFile;
	files.push_back(tempFileNode);

	files[subFile->id].name = FileSystem::getAvailableFileName(subFile, files[subFile->id].name.c_str());
	files[subFile->id].path = files[subFile->parent->id].path + "\\" + files[subFile->id].name + files[subFile->id].extension;
	FileSystem::insertFileToParentsSubfolders(subFile);
	FileSystem::updateChildrenPathRecursively(subFile);

	const auto copyOptions = std::filesystem::copy_options::recursive;
	std::filesystem::copy(files[id].path, files[subFile->id].path, copyOptions);

	if (files[subFile->id].type == FileType::folder) {

		generateFileStructure(subFile);

		std::vector<int> indices;
		FileSystem::getTreeIndices(subFile, indices);

		loadFilesToEngine(indices);
	}
	else
		loadFileToEngine(files[subFile->id]);

	return subFile->id;
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
	else if (extension == ".frag")
		return FileType::fragshader;
	else if (extension == ".vert")
		return FileType::vertshader;
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

		TextureFile texture(fileNode.addr, fileNode.path.c_str());
		textures.insert({ fileNode.path, texture });
		fileNode.textureID = texture.textureID;
		break;
	}
	case FileType::object: {

		Model model(fileNode.path.c_str(), fileNode.addr, this);
		fileNode.textureID = editorTextures.objectBigTextureID;
		break;
	}
	case FileType::folder: {

		fileNode.textureID = editorTextures.folderBigTextureID;
		break;
	}
	case FileType::material: {

		fileNode.textureID = editorTextures.materialTextureID;
		readMaterialFile(fileNode.addr, fileNode.path);
		break;
	}
	case FileType::fragshader: {

		ShaderFile shaderFile(fileNode.addr, fileNode.path);
		fragShaders[fileNode.path] = shaderFile;
		fileNode.textureID = editorTextures.undefinedTextureID;
		break;
	}
	case FileType::vertshader: {

		ShaderFile shaderFile(fileNode.addr, fileNode.path);
		vertShaders[fileNode.path] = shaderFile;
		fileNode.textureID = editorTextures.undefinedTextureID;
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

		if (files[subFile->id].type == FileType::folder) {

			generateFileStructure(subFile);
			std::vector<int> indices;
			FileSystem::getTreeIndices(subFile, indices);
			loadFilesToEngine(indices);
		}
		else
			loadFileToEngine(files[subFile->id]);
	}	
}

void FileSystem::loadDefaultAssets() {

	MeshFile mesh;
	meshes.insert({"Null", mesh });

	MaterialFile mat("source/shader/Default.vert", "source/shader/Default.frag");
	materials.insert({ "Default", mat });

	TextureFile textureWhite("resource/textures/empty_texture_map.DDS");
	textures.insert({ "whitetexture", textureWhite });

	ShaderFile vertDefault;
	vertShaders.insert({ "source/shader/Default.vert", vertDefault });

	ShaderFile fragDefault;
	fragShaders.insert({ "source/shader/Default.frag", fragDefault });
}

MaterialFile& FileSystem::getMaterialFile(int id) {

	return materials.find(files[id].path)->second;
}

TextureFile& FileSystem::getTextureFile(int id) {

	return textures.find(files[id].path)->second;
}

ShaderFile& FileSystem::getFragShaderFile(int id) {

	return fragShaders.find(files[id].path)->second;
}

ShaderFile& FileSystem::getVertShaderFile(int id) {

	return vertShaders.find(files[id].path)->second;
}

MeshFile& FileSystem::getMeshFile(int id) {

	return meshes.find(files[id].path)->second;
}

void FileSystem::setEditor(Editor* editor) { 

	this->editor = editor; 
}