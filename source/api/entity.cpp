#include "entity.hpp"
#include "scene.hpp"

Entity::Entity(const char* name, std::vector<Entity>& entities) {

	this->name = new char[strlen(name) + 1];
	strcpy(this->name, name);
	this->name[strlen(name)] = '\0';
	std::cout << name << std::endl;
	entities.push_back(*this);
}

Entity::Entity(const char* name, Transform* transform, std::vector<Entity>& entities) {

	this->name = (char*)name;
	this->transform = transform;
	entities.push_back(*this);
}

Entity::~Entity() {

}

void Entity::addTransformComponent(Transform* transform) {

	this->transform = transform;
}

void Entity::addMeshRendererComponent(MeshFile* mesh, MaterialFile* mat, std::vector<MeshRenderer>& m_rendererComponents) {

	if (m_rendererComponentIndex != -1)
		return;

	MeshRenderer component;
	component.entID = transform->id;
	component.VAO = mesh->VAO;
	component.indiceSize = mesh->indiceSize;
	component.mat = mat;

	m_rendererComponents.push_back(component);
	m_rendererComponentIndex = m_rendererComponents.size() - 1;
}

void Entity::addLightComponent(std::vector<Light>& lightComponents, Scene* scene, LightType type) {

	if (lightComponentIndex != -1)
		return;
	
	Light component;
	component.entID = transform->id;
	component.type = type;
	component.power = 40.f;
	component.color = glm::vec3(1.f, 1.f, 1.f);

	lightComponents.push_back(component);
	lightComponentIndex = lightComponents.size() - 1;

	if (type == LightType::PointLight)
		scene->pointLightCount++;
	else
		scene->dirLightCount++;

	scene->recompileAllMaterials();
}

void Entity::addRigidbodyComponent(std::vector<Rigidbody>& physicsComponents) {

	if (rigidbodyComponentIndex != -1)
		return;

	Rigidbody comp;
	comp.entID = transform->id;
	physicsComponents.push_back(comp);
	rigidbodyComponentIndex = physicsComponents.size() - 1;
}

void Entity::addMeshColliderComponent(std::vector<MeshCollider>& meshColliderComponents) {

	if (meshColliderComponentIndex != -1)
		return;

	MeshCollider comp;
	comp.entID = transform->id;
	meshColliderComponents.push_back(comp);
	meshColliderComponentIndex = meshColliderComponents.size() - 1;
}

void Entity::removeComponent(ComponentType type, Scene* scene) {

	switch (type) {

	case ComponentType::Light : {

		if (scene->lightComponents[lightComponentIndex].type == LightType::DirectionalLight)
			scene->dirLightCount--;
		else
			scene->pointLightCount--;

		for (int i = lightComponentIndex; i < scene->lightComponents.size() - 1; i++)
			scene->entities[scene->lightComponents[i + 1].entID].lightComponentIndex--;

		scene->lightComponents.erase(scene->lightComponents.begin() + lightComponentIndex);
		lightComponentIndex = -1;

		scene->recompileAllMaterials();
		break;
	}
	case ComponentType::MeshRenderer : {

		for (int i = m_rendererComponentIndex; i < scene->meshRendererComponents.size() - 1; i++)
			scene->entities[scene->meshRendererComponents[i + 1].entID].m_rendererComponentIndex--;

		scene->meshRendererComponents.erase(scene->meshRendererComponents.begin() + m_rendererComponentIndex);
		m_rendererComponentIndex = -1;
		break;
	}
	case ComponentType::Rigidbody: {

		for (int i = rigidbodyComponentIndex; i < scene->rigidbodyComponents.size() - 1; i++)
			scene->entities[scene->rigidbodyComponents[i + 1].entID].rigidbodyComponentIndex--;

		scene->rigidbodyComponents.erase(scene->rigidbodyComponents.begin() + rigidbodyComponentIndex);
		rigidbodyComponentIndex = -1;
		break;
	}
	case ComponentType::MeshCollider: {

		for (int i = meshColliderComponentIndex; i < scene->meshColliderComponents.size() - 1; i++)
			scene->entities[scene->meshColliderComponents[i + 1].entID].meshColliderComponentIndex--;

		scene->meshColliderComponents.erase(scene->meshColliderComponents.begin() + meshColliderComponentIndex);
		meshColliderComponentIndex = -1;
		break;
	}
	}

}