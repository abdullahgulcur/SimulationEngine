#pragma once

#include "GLM/glm.hpp"

#include "PxPhysicsAPI.h"
#include "component.hpp"
#include "material.hpp"

using namespace physx;
using namespace Material;

class Collider : public Component {

public:

	bool trigger = false;;
	PxShape* shape;
	PhysicMaterialFile* pmat;

	Collider();
};

class MeshCollider : public Collider {

public:

	bool convex = false;

	MeshCollider();
};

class BoxCollider : public Collider {

public:

	glm::vec3 center;
	glm::vec3 size;

	BoxCollider();
};

class SphereCollider : public Collider {

public:

	glm::vec3 center;
	float radius = 0.5f;

	SphereCollider();
};

class CapsuleCollider : public Collider {

public:

	glm::vec3 center;
	float radius = 0.5f;
	float height = 1.f;
	int axis = 0;

	CapsuleCollider();
};