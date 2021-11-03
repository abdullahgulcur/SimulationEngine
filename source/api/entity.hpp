#pragma once

#include <string>
#include <unordered_map>

#include "transform.hpp"
#include "component.hpp"
#include "meshrenderer.hpp"

class Entity {

private:

public:

	std::string name;
	Transform* transform;
	std::vector<ComponentType> components;

	Entity();

	void addMeshRendererComponent(Mesh* mesh, Material* mat, std::unordered_map<unsigned int, MeshRenderer>& m_rendererComponents);

	void addLightComponent();
};