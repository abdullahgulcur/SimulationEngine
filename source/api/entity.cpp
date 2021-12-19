#include "entity.hpp"
#include "scene.hpp"
#include "editor.hpp"

// first load
Entity::Entity(const char* name) {

	this->name = new char[strlen(name) + 1];
	strcpy(this->name, name);
	this->name[strlen(name)] = '\0';
	std::cout << name << std::endl;
	transform = new Transform(this);
}

// clone
Entity::Entity(Entity* ent, Transform* parent, Scene& scene) {

	transform = new Transform(this, parent, ent->transform);

	char* name = new char[strlen(ent->name) + 5];
	strcpy(name, ent->name);
	strcat(name, "_cpy\0");
	this->name = name;

	Entity::deepCopyComponents(ent->components, scene);
}

//create empty from parent
Entity::Entity(const char* name, Transform* parent) {

	transform = new Transform(this, parent);

	this->name = new char[strlen(name) + 1];
	strcpy(this->name, name);
	this->name[strlen(name)] = '\0';
}

Entity::~Entity() {

}

Entity* Entity::copy(Scene* scene) {

	Entity* ent = new Entity(this, transform->parent, *scene);
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
			rbComp->body = scene.editor->physics.gPhysics->createRigidDynamic(comp->body->getGlobalPose());
			rbComp->body->setMass(comp->body->getMass());
			rbComp->body->setActorFlags(comp->body->getActorFlags());
			rbComp->body->setRigidBodyFlags(comp->body->getRigidBodyFlags());
			rbComp->body->setRigidDynamicLockFlags(comp->body->getRigidDynamicLockFlags());
			scene.editor->physics.gScene->addActor(*rbComp->body);

			for (auto& collider : getComponents<Collider>())
				rbComp->body->attachShape(*collider->shape);

		}
		else if (BoxCollider* comp = dynamic_cast<BoxCollider*>(it))
		{
			BoxCollider* boxColliderComp = addComponent<BoxCollider>();
			boxColliderComp->center = comp->center;
			boxColliderComp->size = comp->size;
			boxColliderComp->pmat = comp->pmat;
			boxColliderComp->pmat->colliderCompAddrs.push_back(boxColliderComp);
			glm::vec3 size = transform->globalScale * boxColliderComp->size / 2.f;
			boxColliderComp->shape = scene.editor->physics.gPhysics->createShape(PxBoxGeometry(size.x, size.y, size.z), *boxColliderComp->pmat->pxmat);
			boxColliderComp->shape->setFlags(comp->shape->getFlags());
			glm::vec3 center = boxColliderComp->transform->globalScale * boxColliderComp->center;
			boxColliderComp->shape->setLocalPose(PxTransform(center.x, center.y, center.z));

			if (Rigidbody* rb = getComponent<Rigidbody>())
				rb->body->attachShape(*boxColliderComp->shape);

		}
		else if (SphereCollider* comp = dynamic_cast<SphereCollider*>(it))
		{
			SphereCollider* sphereColliderComp = addComponent<SphereCollider>();
			sphereColliderComp->center = comp->center;
			sphereColliderComp->radius = comp->radius;
			sphereColliderComp->shape = comp->shape;
			sphereColliderComp->pmat = comp->pmat;
			sphereColliderComp->pmat->colliderCompAddrs.push_back(sphereColliderComp);
			sphereColliderComp->shape = scene.editor->physics.gPhysics->createShape(PxSphereGeometry(sphereColliderComp->radius), *sphereColliderComp->pmat->pxmat);
			glm::vec3 center = sphereColliderComp->transform->globalScale * sphereColliderComp->center;
			sphereColliderComp->shape->setLocalPose(PxTransform(center.x, center.y, center.z));

			if (Rigidbody* rb = getComponent<Rigidbody>())
				rb->body->attachShape(*sphereColliderComp->shape);
		}
		else if (CapsuleCollider* comp = dynamic_cast<CapsuleCollider*>(it))
		{
			CapsuleCollider* capsuleColliderComp = addComponent<CapsuleCollider>();
			capsuleColliderComp->axis = comp->axis;
			capsuleColliderComp->height = comp->height;
			capsuleColliderComp->center = comp->center;
			capsuleColliderComp->radius = comp->radius;
			capsuleColliderComp->shape = comp->shape;
			capsuleColliderComp->pmat = comp->pmat;
			capsuleColliderComp->pmat->colliderCompAddrs.push_back(capsuleColliderComp);
		}
		else if (MeshCollider* comp = dynamic_cast<MeshCollider*>(it))
		{
			MeshCollider* meshColliderComp = addComponent<MeshCollider>();
			meshColliderComp->convex = comp->convex;
			meshColliderComp->shape = comp->shape;
			meshColliderComp->pmat = comp->pmat;
			meshColliderComp->pmat->colliderCompAddrs.push_back(meshColliderComp);
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

	if (Light* lightComp = getComponent<Light>()) {

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

	//for (auto& comp : getComponents<Collider>())
	//	comp->shape->

	if (Rigidbody* rb = getComponent<Rigidbody>())
		scene->editor->physics.gScene->removeActor(*rb->body);

	delete[] name;
	delete transform;

	for (auto& it : components)
		delete it;

	delete this;

	return true;
}