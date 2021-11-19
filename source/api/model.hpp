#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.hpp"

#include <string>
#include <iostream>

class FileSystem;

class Model {

private:

public:

	Model(std::string const& path, File* file, FileSystem* fileSystem);

	void loadModel(std::string const& path, File* file, FileSystem* fileSystem);

	void processNode(std::string const& path, aiNode* node, const aiScene* scene, File* file, FileSystem* fileSystem);

	void processMesh(std::string const& path, aiMesh* mesh, const aiScene* scene, File* file, FileSystem* fileSystem);

};