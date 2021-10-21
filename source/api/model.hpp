#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh_renderer.hpp"

#include <string>
#include <iostream>

class Model {

private:

public:

	std::vector<MeshRenderer> meshes;

	Model();

	void loadModel(std::string const& path);

	void processNode(aiNode* node, const aiScene* scene);

	MeshRenderer processMesh(aiMesh* mesh, const aiScene* scene);

};