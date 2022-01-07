#pragma once

#include "mesh.hpp"
#include "material.hpp"
#include "component.hpp"

using namespace Material;

class MeshRenderer: public Component {

public:

	Mesh::MeshFile* mesh;
	MaterialFile* mat;

	MeshRenderer();

	MeshRenderer(Mesh::MeshFile* mesh);
};