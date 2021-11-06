#pragma once

#include "mesh.hpp"
#include "material.hpp"

class MeshRenderer {

public:

	unsigned int entID;

	int VAO;
	int indiceSize;
	Material* mat;

	MeshRenderer();
};