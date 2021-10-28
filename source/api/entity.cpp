#include "entity.hpp"
#include "scene.hpp"

Entity::Entity() {

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

	for (int i = 0; i < components.size(); i++) {

		if (components[i] == ComponentType::Light)
			return;
	}

	LightComponent component;
	component.id = transform->id;
	component.transform = transform;
	component.type = LightType::DirectionalLight;
	component.power = 40.f;
	component.color = glm::vec3(1.f, 1.f, 1.f);
	scene->lightComponents[transform->id] = component;
	components.push_back(ComponentType::Light);
}

void Entity::addMeshRendererComponent() {

	for (int i = 0; i < components.size(); i++) {

		if (components[i] == ComponentType::MeshRenderer)
			return;
	}

	MeshRendererComponent component;
	component.meshName = "Null";
	component.id = transform->id;
	component.transform = transform;
	scene->meshRendererComponents[transform->id] = component;

	components.push_back(ComponentType::MeshRenderer);
}

void Entity::addRigidBodyComponent() {

	components.push_back(ComponentType::RigidBody);
}

void Entity::addScriptComponent() {

	components.push_back(ComponentType::Script);
}

void Entity::removeComponent(ComponentType type) {

	switch (type) {

	case ComponentType::Light:
		scene->lightComponents.erase(transform->id);
		break;
	case ComponentType::MeshRenderer:
		scene->meshRendererComponents.erase(transform->id);
		break;
	}

	for (int i = 0; i < components.size(); i++) {

		if (components[i] == type) {

			components.erase(components.begin() + i);
			return;
		}
	}

}

//void Entity::removeAnimationComponent() {
//
//
//}
//
//void Entity::removeAnimatorComponent() {
//
//
//}
//
//void Entity::removeColliderComponent() {
//
//
//}
//
//void Entity::removeLightComponent() {
//
//
//}
//
//void Entity::removeMeshRendererComponent() {
//
//
//}
//
//void Entity::removeRigidBodyComponent() {
//
//
//}
//
//void Entity::removeScriptComponent() {
//
//
//}

void Entity::setScene(Scene* scene) { this->scene = scene; }
