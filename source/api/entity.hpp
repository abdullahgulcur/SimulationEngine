#pragma once

#include <string>

#include "transform.hpp"
#include "component.hpp"

class Scene;
class Editor;

class Entity {

private:

public:

	std::string name;
	Transform* transform;
	std::vector<ComponentType> components;

	Entity();

	void addMeshRendererComponent(Mesh& mesh, std::vector<MeshRendererComponent>& m_rendererComponents);

	void addLightComponent();
};