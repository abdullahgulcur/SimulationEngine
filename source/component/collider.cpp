#include "collider.hpp"

MeshCollider::MeshCollider() {

}

BoxCollider::BoxCollider() {

	center = glm::vec3(0.f, 0.f, 0.f);
	size = glm::vec3(1.f, 1.f, 1.f);
}

SphereCollider::SphereCollider() {

	center = glm::vec3(0.f, 0.f, 0.f);
}

CapsuleCollider::CapsuleCollider() {

	center = glm::vec3(0.f, 0.f, 0.f);
}