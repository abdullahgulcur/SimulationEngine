#include "collider.hpp"
#include "entity.hpp"
#include "meshrenderer.hpp"

Collider::Collider() {

}

void Collider::updatePoseGeometry() {

}

void Collider::updatePoseGeometryAndRelease() {

}

MeshCollider::MeshCollider() {

}

BoxCollider::BoxCollider() {

	center = glm::vec3(0.f, 0.f, 0.f);
	size = glm::vec3(1.f, 1.f, 1.f);
}

// todo: only do when collider props or transform's scale is changed 
void BoxCollider::updatePoseGeometry() {

	glm::vec3 size = transform->globalScale * this->size / 2.f;
	glm::vec3 center = transform->globalScale * this->center;
	shape->setLocalPose(PxTransform(center.x, center.y, center.z));
	shape->setGeometry(PxBoxGeometry(size.x, size.y, size.z));
}

void BoxCollider::updatePoseGeometryAndRelease() {

	glm::vec3 size = transform->globalScale * this->size / 2.f;
	glm::vec3 center = transform->globalScale * this->center;
	shape->setLocalPose(PxTransform(center.x, center.y, center.z));
	shape->setGeometry(PxBoxGeometry(size.x, size.y, size.z));
	shape->release();
}

// update only pose !!!

SphereCollider::SphereCollider() {

	center = glm::vec3(0.f, 0.f, 0.f);
}

void SphereCollider::updatePoseGeometry() {

	glm::vec3 center = transform->globalScale * this->center;
	shape->setLocalPose(PxTransform(center.x, center.y, center.z));
	float max = transform->globalScale.x > transform->globalScale.y ? transform->globalScale.x : transform->globalScale.y;
	max = max > transform->globalScale.z ? max : transform->globalScale.z;
	shape->setGeometry(PxSphereGeometry(radius * max));
}

void SphereCollider::updatePoseGeometryAndRelease() {

	glm::vec3 center = transform->globalScale * this->center;
	shape->setLocalPose(PxTransform(center.x, center.y, center.z));
	float max = transform->globalScale.x > transform->globalScale.y ? transform->globalScale.x : transform->globalScale.y;
	max = max > transform->globalScale.z ? max : transform->globalScale.z;
	shape->setGeometry(PxSphereGeometry(radius * max));
	shape->release();
}

CapsuleCollider::CapsuleCollider() {

	center = glm::vec3(0.f, 0.f, 0.f);
}

void CapsuleCollider::updatePoseGeometry() {

}

void CapsuleCollider::updatePoseGeometryAndRelease() {

}