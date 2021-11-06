#include "entity.hpp"
#include "scene.hpp"

Entity::Entity() {

}

Entity::~Entity() {

	//std::cout << "\nEntity fucked up with the id " << transform->id << " !!!\n" << std::endl;
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

void Entity::addLightComponent(std::unordered_map<unsigned int, Light>& lightComponents, Scene* scene) {

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

	scene->recompileAllMaterials();
}

void Entity::removeComponent(ComponentType type, Scene* scene) {

	switch (type) {

	case ComponentType::Light : {

		if (scene->lightComponents[transform->id].type == LightType::DirectionalLight)
			scene->dirLightCount--;
		else
			scene->pointLightCount--;

		scene->lightComponents.erase(transform->id);
		scene->recompileAllMaterials();
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