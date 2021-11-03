#pragma once

#include <string>
#include <filesystem>
#include <iostream>
#include <map>
#include <unordered_map>
#include <stack>
#include <shlobj.h>
#include <fstream>

#include "rapidxml_print.hpp"
#include "rapidxml.hpp"

#include "model.hpp"
#include "texture.hpp"
#include "utility.hpp"
#include "material.hpp"

class Editor;

enum class FileType {folder, object, material, texture, audio, script, undefined};

struct EditorTextures {

	unsigned int openFolderTextureID;
	unsigned int closedFolderTextureID;
	unsigned int objectTextureID;
	unsigned int objectBigTextureID;
	unsigned int textureTextureID;
	unsigned int undefinedTextureID;
	unsigned int documentTextureID;
	unsigned int folderBigTextureID;
	unsigned int plusTextureID;
	unsigned int materialTextureID;
};

struct File {

	int id;
	File* parent;
	std::vector<File*> subfiles;
};

struct FileNode {

	std::string path;
	std::string name;
	std::string extension;
	FileType type;
	File* addr;
	unsigned int textureID;
};

class FileSystem {

private:

	Editor* editor;

	void initEditorTextures();

	void generateFileStructure(File* file);

	unsigned int getSubFileIndex(File* file);

	void insertFileToParentsSubfolders(File* file);

	std::string getAvailableFileName(File* file, const char* name);

	void loadTextureIDsOfMaterials();

public:

	std::string assetsPathExternal;

	File* rootFile;
	std::vector<FileNode> files;

	std::unordered_map<std::string, Mesh> meshes;
	std::unordered_map<std::string, Material> materials;
	std::unordered_map<std::string, Texture> textures;

	EditorTextures editorTextures;

	FileSystem();

	~FileSystem();

	void checkProjectFolder();

	void initFileSystem();

	void updateChildrenPathRecursively(File* file);

	void traverseAllFiles(File* file);

	void getTreeIndices(File* file, std::vector<int>& indices);

	bool subfolderAndItselfCheck(File* fileToMove, File* fileToBeMoved);

	bool subfolderCheck(File* fileToMove, File* fileToBeMoved);

	void moveFile(int toBeMoved, int moveTo);

	void changeAssetsKeyManually(int toBeMoved, std::string previousName, std::string newName);

	void deleteFileFromTree(File* parent, int id);

	void deleteFileCompletely(int id);

	void duplicateFile(int id);

	FileType getFileType(std::string extension);

	void newFolder(int currentDirID, const char* fileName);

	void newMaterial(int currentDirID, const char* fileName);

	void readMaterialFile(std::string path, Material& mat);

	void writeMaterialFile(std::string path, Material& mat);

	void rename(int id, const char* newName);

	bool hasSubFolder(File* file);

	void loadFileToEngine(FileNode& fileNode);

	void importFiles(std::vector<std::string> filesToMove, int toDir);

	void loadDefaultAssets();

	Material& getMaterial(int id);

	Texture& getTexture(int id);

	void setEditor(Editor* editor);



	//node* newNode(int data);

	//void identicalTrees(node* a, node* b, bool& identical);

	//void detectFilesChangedOutside(File* file, std::string fileToCompare);

};