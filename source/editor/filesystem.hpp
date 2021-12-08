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
#include "shader.hpp"
//#include "log.hpp"

using namespace Material;
using namespace TextureNS;
using namespace ShaderNS;
using namespace Mesh;

class Editor;

enum class FileType {folder, object, material, texture, fragshader, vertshader, audio, script, undefined};

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

	void loadFilesToEngine();

	void loadFilesToEngine(std::vector<int>& indices);

	unsigned int getSubFileIndex(File* file);

	void insertFileToParentsSubfolders(File* file);

	std::string getAvailableFileName(File* file, const char* name);

public:

	std::string assetsPathExternal;

	File* rootFile;
	std::vector<FileNode> files;

	std::unordered_map<std::string, MeshFile> meshes;
	std::unordered_map<std::string, MaterialFile> materials;
	std::unordered_map<std::string, TextureFile> textures;
	std::unordered_map<std::string, ShaderFile> vertShaders;
	std::unordered_map<std::string, ShaderFile> fragShaders;

	EditorTextures editorTextures;

	FileSystem();

	~FileSystem();

	void checkProjectFolder();

	void init(Editor* editor);

	void updateChildrenPathRecursively(File* file);

	void traverseAllFiles(File* file);

	void getTreeIndices(File* file, std::vector<int>& indices);

	bool subfolderAndItselfCheck(File* fileToMove, File* fileToBeMoved);

	bool subfolderCheck(File* fileToMove, File* fileToBeMoved);

	void moveFile(int toBeMoved, int moveTo);

	void changeAssetsKeyManually(int filID, std::string previousName, std::string newName);

	void deleteFileFromTree(File* parent, int id);

	void deleteFileCompletely(int id);

	int duplicateFile(int id);

	FileType getFileType(std::string extension);

	int newFolder(int currentDirID, const char* fileName);

	void newMaterial(int currentDirID, const char* fileName);

	void readMaterialFile(File* filePtr, std::string path);

	void writeMaterialFile(std::string path, MaterialFile& mat);

	File* getTextureFileAddr(const char* path);

	const char* getTextureFilePath(File* addr);

	File* getFragShaderAddr(const char* path);

	File* getVertShaderAddr(const char* path);

	const char* getFragShaderPath(File* fileAddr);

	const char* getVertShaderPath(File* fileAddr);

	void rename(int id, const char* newName);

	bool hasSubFolder(File* file);

	void loadFileToEngine(FileNode& fileNode);

	void importFiles(std::vector<std::string> filesToMove, int toDir);

	void loadDefaultAssets();

	MaterialFile& getMaterialFile(int id);

	TextureFile& getTextureFile(int id);

	ShaderFile& getFragShaderFile(int id);

	ShaderFile& getVertShaderFile(int id);

	MeshFile& getMeshFile(int id);

	void setEditor(Editor* editor);
};