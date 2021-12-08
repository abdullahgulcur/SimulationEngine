#include "collider.hpp"

MeshCollider::MeshCollider() {

	//type = ComponentType::MeshCollider;
}

BoxCollider::BoxCollider() {

	//type = ComponentType::BoxCollider;

	center = glm::vec3(0.f, 0.f, 0.f);
	size = glm::vec3(1.f, 1.f, 1.f);
}

SphereCollider::SphereCollider() {

	//type = ComponentType::SphereCollider;

	center = glm::vec3(0.f, 0.f, 0.f);
}

CapsuleCollider::CapsuleCollider() {

	//type = ComponentType::CapsuleCollider;

	center = glm::vec3(0.f, 0.f, 0.f);
}