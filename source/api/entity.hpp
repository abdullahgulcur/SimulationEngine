#pragma once

#include <string>

#include "transform.hpp"
#include "component.hpp"

class Scene;

class Entity {

private:

public:

	Scene* scene;
	std::string name;
	Transform* transform;
	std::vector<ComponentType> components;

	Entity();

	void addComponent(ComponentType type);

	void setScene(Scene* scene);

	void addAnimationComponent();

	void addAnimatorComponent();

	void addColliderComponent();

	void addLightComponent();

	void addMeshRendererComponent(std::string name);

	void addRigidBodyComponent();

	void addScriptComponent();

};