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

	void setScene(Scene* scene);

	/*
	* Add Components
	*/

	void addAnimationComponent();

	void addAnimatorComponent();

	void addColliderComponent();

	void addLightComponent();

	void addMeshRendererComponent();

	void addRigidBodyComponent();

	void addScriptComponent();

	/*
	* Remove Components
	*/

	void removeComponent(ComponentType type);

	//void removeAnimationComponent();

	//void removeAnimatorComponent();

	//void removeColliderComponent();

	//void removeLightComponent();

	//void removeMeshRendererComponent();

	//void removeRigidBodyComponent();

	//void removeScriptComponent();

};