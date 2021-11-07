#pragma once

#include <string>
#include <unordered_map>

#include "transform.hpp"
#include "component.hpp"
#include "meshrenderer.hpp"
#include "light.hpp"

class Scene;

class Entity {

private:

public:

	std::string name;
	Transform* transform;

	unsigned int m_rendererComponentIndex = -1;
	unsigned int lightComponentIndex = -1;

	Entity();

	~Entity();

	void addMeshRendererComponent(Mesh* mesh, Material* mat, std::vector<MeshRenderer>& m_rendererComponents);

	void addLightComponent(std::vector<Light>& lightComponents, Scene* scene, LightType type = LightType::DirectionalLight);

	void removeComponent(ComponentType type, Scene* scene);
};