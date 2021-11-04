#include "entity.hpp"
#include "scene.hpp"

Entity::Entity() {

}

void Entity::addMeshRendererComponent(Mesh* mesh, Material* mat, std::unordered_map<unsigned int, MeshRenderer>& m_rendererComponents) {

	for (int i = 0; i < components.size(); i++) {

		if (components[i] == ComponentType::MeshRenderer)
			return;
	}

	MeshRenderer component;
	component.entID = transform->id;
	component.mesh = mesh;
	component.mat = mat;

	m_rendererComponents.insert({ component.entID, component });

	components.push_back(ComponentType::MeshRenderer);
}

void Entity::addLightComponent(std::unordered_map<unsigned int, Light>& lightComponents) {

	for (int i = 0; i < components.size(); i++) {
	
		if (components[i] == ComponentType::Light)
			return;
	}
	
	Light component;
	component.entID = transform->id;
	component.type = LightType::DirectionalLight;
	component.power = 40.f;
	component.color = glm::vec3(1.f, 1.f, 1.f);

	lightComponents.insert({ component.entID, component });

	components.push_back(ComponentType::Light);
}

void Entity::removeComponent(ComponentType type, Scene* scene) {

	switch (type) {

	case ComponentType::Light : {

		scene->lightComponents.erase(transform->id);
		break;
	}
	case ComponentType::MeshRenderer : {

		scene->meshRendererComponents.erase(transform->id);
		break;
	}
	}

	for (int i = 0; i < components.size(); i++) {

		if (components[i] == type) {
			components.erase(components.begin() + i);
			return;
		}
	}
}