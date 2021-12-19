#pragma once

#include "GLM/glm.hpp"

#include "PxPhysicsAPI.h"
#include "component.hpp"
#include "material.hpp"
#include "shader.hpp"

using namespace physx;
using namespace Material;

class Collider : public Component {

public:

	PxShape* shape;
	PhysicMaterialFile* pmat;

	Collider();

	virtual void updatePoseGeometry();

	virtual void updatePoseGeometryAndRelease();
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

	void updatePoseGeometry();

	void updatePoseGeometryAndRelease();
};

class SphereCollider : public Collider {

public:

	glm::vec3 center;
	float radius = 0.5f;

	SphereCollider();

	void updatePoseGeometry();

	void updatePoseGeometryAndRelease();
};

class CapsuleCollider : public Collider {

public:

	glm::vec3 center;
	float radius = 0.5f;
	float height = 1.f;
	int axis = 0;

	CapsuleCollider();

	void updatePoseGeometry();

	void updatePoseGeometryAndRelease();

};