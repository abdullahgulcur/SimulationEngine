#pragma once

#include <string>

#include "transform.hpp"
#include "component.hpp"

class Scene;
class Editor;

class Entity {

private:

public:

	Scene* scene;
	Editor* editor;
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

	/*
	* Update Components
	*/

	void updateLightComponent();

	void updateMeshRendererComponent(MeshRenderer* renderer);

};