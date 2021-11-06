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

	std::vector<Mesh> meshes;

	Model(std::string const& path, FileSystem* fileSystem);

	void loadModel(std::string const& path, FileSystem* fileSystem);

	void processNode(aiNode* node, const aiScene* scene, FileSystem* fileSystem);

	Mesh processMesh(aiMesh* mesh, const aiScene* scene, FileSystem* fileSystem);

};