#pragma once

#include <api/model.hpp>
#include <api/texture.hpp>
#include <api/utility.hpp>

#include <string>
#include <filesystem>
#include <iostream>
#include <map>
#include <stack>
#include <shlobj.h>

enum class FileType {folder, object, texture, audio, script, undefined};

enum class AddType { fromMove, fromRename, fromNewFolder, fromImport, fromDuplicate };


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

class node
{
public:
	int data;
	node* left;
	node* right;
};

class FileSystem {

private:

	int index = 0;
	Texture texture;
	std::string assetsPathExternal;

	void addFile(File* file, const char* name, AddType type);

public:

	File* file;
	std::map<int, FileNode> files;
	std::vector<Model> models;
	std::vector<unsigned int> textures;
	EditorTextures editorTextures;

	FileSystem();

	void checkAssetsFolder();

	void initFileSystem();

	void initEditorTextures();

	void generateFileStructure(File* file);

	void updateChildrenPathRecursively(File* file);

	void traverseAllFiles(File* file);

	bool subfolderAndItselfCheck(File* fileToMove, File* fileToBeMoved);

	bool subfolderCheck(File* fileToMove, File* fileToBeMoved);

	void moveFile(int toBeMoved, int moveTo);

	void deleteFileFromTree(File* parent, int id);

	void deleteFileCompletely(int id);

	void duplicateFile(int id);

	FileType getFileType(std::string extension);

	void newFolder(int currentDirID, const char* fileName);

	void rename(int id, const char* newName);

	bool hasSubFolder(File* file);

	void loadFileToEngine(FileNode& fileNode);

	void importFiles(std::vector<std::string> filesToMove, int toDir);

	node* newNode(int data);

	void identicalTrees(node* a, node* b, bool& identical);

	void detectFilesChangedOutside(File* file, std::string fileToCompare);

};