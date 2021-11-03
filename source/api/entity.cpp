#include "entity.hpp"

Entity::Entity() {

}

void Entity::addMeshRendererComponent(Mesh& mesh, std::vector<MeshRendererComponent>& m_rendererComponents) {

	for (int i = 0; i < components.size(); i++) {

		if (components[i] == ComponentType::MeshRenderer)
			return;
	}

	MeshRendererComponent component;
	component.entID = transform->id;
	component.VAO = mesh.VAO;
	component.indiceCount = mesh.indices.size();
	m_rendererComponents.push_back(component);

	//component.meshName = "Null";
	//component.id = transform->id;
	//component.transform = transform;
	//scene->meshRendererComponents[transform->id] = component;

	components.push_back(ComponentType::MeshRenderer);
}

void Entity::addLightComponent() {

	//	for (int i = 0; i < components.size(); i++) {
	//
	//		if (components[i] == ComponentType::Light)
	//			return;
	//	}
	//
	//	LightComponent component;
	//	component.id = transform->id;
	//	component.transform = transform;
	//	component.type = LightType::DirectionalLight;
	//	component.power = 40.f;
	//	component.color = glm::vec3(1.f, 1.f, 1.f);
	//	scene->lightComponents[transform->id] = component;
	//	components.push_back(ComponentType::Light);
}