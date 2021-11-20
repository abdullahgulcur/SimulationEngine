#pragma once

#include "mesh.hpp"
#include "material.hpp"

using namespace MaterialNS;

class MeshRenderer {

public:

	unsigned int entID;

	unsigned int VAO;
	int indiceSize;
	MaterialFile* mat;

	MeshRenderer();
};