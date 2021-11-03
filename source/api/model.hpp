#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.hpp"

#include <string>
#include <iostream>

class Model {

private:

public:

	std::vector<Mesh> meshes;

	Model();

	void loadModel(std::string const& path);

	void processNode(aiNode* node, const aiScene* scene);

	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

};