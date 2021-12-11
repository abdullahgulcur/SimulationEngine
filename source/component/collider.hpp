#pragma once

#include "GLM/glm.hpp"

#include "physics.hpp"
#include "component.hpp"

using namespace physx;

class MeshCollider : public Component {

public:

	bool convex = false;
	bool trigger = false;
	PxShape* shape;
	PxMaterial* mat;

	MeshCollider();
};

class BoxCollider : public Component {

public:

	bool trigger = false;
	glm::vec3 center;
	glm::vec3 size;
	PxShape* shape;
	PxMaterial* mat;

	BoxCollider();
};

class SphereCollider : public Component {

public:

	bool trigger = false;
	glm::vec3 center;
	float radius = 0.5f;
	PxShape* shape;
	PxMaterial* mat;

	SphereCollider();
};

class CapsuleCollider : public Component {

public:

	bool trigger = false;
	glm::vec3 center;
	float radius = 0.5f;
	float height = 1.f;
	int axis = 0;
	PxShape* shape;
	PxMaterial* mat;

	CapsuleCollider();
};