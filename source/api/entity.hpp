#pragma once

#include <string>
#include <unordered_map>

#include "transform.hpp"
#include "meshrenderer.hpp"
#include "mesh.hpp"
#include "light.hpp"
#include "physics_comp.hpp"

using namespace MaterialNS;
using namespace Mesh;

enum class ComponentType{Light, MeshRenderer, Physics, Collider, Transform, Script, Animation, Animator};

class Scene;

class Entity {

private:

public:

	char* name;
	Transform* transform;

	unsigned int m_rendererComponentIndex = -1;
	unsigned int lightComponentIndex = -1;
	unsigned int physicsComponentIndex = -1;

	Entity(const char* name, std::vector<Entity>& entities);

	Entity(const char* name, Transform* transform, std::vector<Entity>& entities);

	~Entity();

	void addTransformComponent(Transform* transform);

	void addMeshRendererComponent(MeshFile* mesh, MaterialFile* mat, std::vector<MeshRenderer>& m_rendererComponents);

	void addLightComponent(std::vector<Light>& lightComponents, Scene* scene, LightType type = LightType::DirectionalLight);

	void addPhysicsComponent(std::vector<PhysicsComponent>& physicsComponents);

	void removeComponent(ComponentType type, Scene* scene);
};