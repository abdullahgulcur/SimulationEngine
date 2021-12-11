#include "entity.hpp"
#include "scene.hpp"

// new ent
Entity::Entity(const char* name, std::vector<Entity>& entities) {

	this->name = new char[strlen(name) + 1];
	strcpy(this->name, name);
	this->name[strlen(name)] = '\0';
	std::cout << name << std::endl;
	transform = new Transform(this);
	//entities.push_back(*this);
}

// clone
Entity::Entity(Entity* ent, Transform* parent, Scene& scene) {

	transform = new Transform(this, parent, ent->transform, scene.entities.size());

	char* name = new char[strlen(ent->name) + 5];
	strcpy(name, ent->name);
	strcat(name, "_cpy\0");
	this->name = name;

	Entity::deepCopyComponents(ent->components, scene);
	//scene.entities.push_back(*this);
}

//create empty from parent
Entity::Entity(const char* name, Entity* parent, Scene& scene) {

	transform = new Transform(this, parent->transform, scene.entities.size());

	this->name = new char[strlen(name) + 1];
	strcpy(this->name, name);
	this->name[strlen(name)] = '\0';

	//scene.entities.push_back(*this);
}

Entity::~Entity() {

}

Entity Entity::copy(Scene* scene) {

	Entity ent(&scene->entities[transform->id], transform->parent, *scene);
	return ent;
}

// internal usage only
void Entity::deepCopyComponents(std::vector<Component*> components, Scene& scene) {

	for (auto& it : components) {

		if (MeshRenderer* comp = dynamic_cast<MeshRenderer*>(it))
		{
			MeshRenderer* meshRendererComp = addComponent<MeshRenderer>();
			meshRendererComp->mesh = comp->mesh;
			meshRendererComp->mat = comp->mat;
			comp->mat->meshRendererCompAddrs.push_back(meshRendererComp);
			comp->mesh->meshRendererCompAddrs.push_back(meshRendererComp);
		}
		else if (Rigidbody* comp = dynamic_cast<Rigidbody*>(it))
		{
			Rigidbody* rbComp = addComponent<Rigidbody>();
			rbComp->body = comp->body;
			rbComp->freezePos = comp->freezePos;
			rbComp->freezeRot = comp->freezeRot;
			rbComp->isKinematic = comp->isKinematic;
			rbComp->mass = comp->mass;
			rbComp->useGravity = comp->useGravity;
		}
		else if (MeshCollider* comp = dynamic_cast<MeshCollider*>(it))
		{
			MeshCollider* meshColliderComp = addComponent<MeshCollider>();
			meshColliderComp->convex = comp->convex;
			meshColliderComp->shape = comp->shape;
			meshColliderComp->trigger = comp->trigger;
		}
		else if (Light* comp = dynamic_cast<Light*>(it))
		{
			Light* lightComp = addComponent<Light>();
			lightComp->color = comp->color;
			lightComp->lightType = comp->lightType;
			lightComp->power = comp->power;

			if (lightComp->lightType == LightType::DirectionalLight)
				scene.dirLightTransforms.push_back(transform);
			else
				scene.pointLightTransforms.push_back(transform);

			scene.recompileAllMaterials();
		}
	}
}

bool Entity::destroy(Scene* scene) {

	Light* lightComp = getComponent<Light>();
	if (lightComp != nullptr) {

		if (lightComp->lightType == LightType::DirectionalLight) {

			for (auto it = scene->dirLightTransforms.begin(); it < scene->dirLightTransforms.end(); it++) {

				if (*it == transform) {
					scene->dirLightTransforms.erase(it);
					scene->recompileAllMaterials();
					break;
				}
			}
		}
		else {

			for (auto it = scene->pointLightTransforms.begin(); it < scene->pointLightTransforms.end(); it++) {

				if (*it == transform) {
					scene->pointLightTransforms.erase(it);
					scene->recompileAllMaterials();
					break;
				}
			}
		}
	}

	delete[] name;
	delete transform;

	for (auto& it : components)
		delete it;

	return true;
}

//void Entity::addRigidbodyComponent(std::vector<Rigidbody>& physicsComponents,
//	std::vector<MeshRenderer>& m_rendererComponents, Physics* physics) {
//
//	if (rigidbodyComponentIndex != -1)
//		return;
//
//	Rigidbody comp;
//	comp.entID = transform->id;
//	physicsComponents.push_back(comp);
//	rigidbodyComponentIndex = physicsComponents.size() - 1;
//
//	for (auto& it : m_rendererComponents) {
//
//		if (it.entID == transform->id) {
//
//			physics->addConvexMesh(&it, transform, &physicsComponents[rigidbodyComponentIndex]);
//			break;
//		}
//	}
//
//}
//
//void Entity::addMeshColliderComponent(std::vector<MeshCollider>& meshColliderComponents, 
//	std::vector<MeshRenderer>& m_rendererComponents, Physics* physics) {
//
//	if (meshColliderComponentIndex != -1)
//		return;
//
//	MeshCollider comp;
//	comp.entID = transform->id;
//	meshColliderComponents.push_back(comp);
//	meshColliderComponentIndex = meshColliderComponents.size() - 1;
//}
