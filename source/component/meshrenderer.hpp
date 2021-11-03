#pragma once

#include "mesh.hpp"
#include "material.hpp"

class MeshRenderer {

public:

	unsigned int entID;

	Mesh* mesh;
	Material* mat;

	MeshRenderer();
};