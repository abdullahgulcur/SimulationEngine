#include "meshrenderer.hpp"

MeshRenderer::MeshRenderer() {

}

MeshRenderer::MeshRenderer(Mesh::MeshFile* mesh) {

	this->mesh = mesh;
}