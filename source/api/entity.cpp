#include "entity.hpp"
#include "scene.hpp"

Entity::Entity() {

}

// From scracth
void Entity::addComponent(ComponentType type) {

	for (int i = 0; i < components.size(); i++) {

		if (components[i] == type)
			return;
	}

	switch (type) {

	case ComponentType::Animation:
		//
		break;
	case ComponentType::Animator:
		//
		break;
	case ComponentType::Collider:
		//
		break;
	case ComponentType::Light:
		//
		break;
	case ComponentType::MeshRenderer: {

		MeshRendererComponent component;
		component.meshName = "Null";
		component.id = transform->id;
		component.transform = transform;
		scene->meshRendererComponents[transform->id] = component;
		break;
	}

	case ComponentType::RigidBody:
		//
		break;
	case ComponentType::Script:
		//
		break;
	case ComponentType::Transform:
		//
		break;
	}

	components.push_back(type);
}

void Entity::addAnimationComponent() {

	components.push_back(ComponentType::Animation);
}

void Entity::addAnimatorComponent() {

	components.push_back(ComponentType::Animator);
}

void Entity::addColliderComponent() {

	components.push_back(ComponentType::Collider);
}

void Entity::addLightComponent() {

	components.push_back(ComponentType::Light);
}

void Entity::addMeshRendererComponent(std::string name) {

	for (int i = 0; i < components.size(); i++) {

		if (components[i] == ComponentType::MeshRenderer)
			return;
	}

	/*MeshRendererComponent component;
	component.meshName = name;
	component.id = transform->id;
	component.transform = transform;
	scene->meshRendererComponents[transform->id] = component;*/

	MeshRendererComponent component;
	component.meshName = name;
	component.id = transform->id;
	//component.transform = transform;
	scene->meshRendererComponents[transform->id] = component;

	components.push_back(ComponentType::MeshRenderer);
}

void Entity::addRigidBodyComponent() {

	components.push_back(ComponentType::RigidBody);
}

void Entity::addScriptComponent() {

	components.push_back(ComponentType::Script);
}

void Entity::setScene(Scene* scene) { this->scene = scene; }
