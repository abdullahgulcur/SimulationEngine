#pragma once

#include "GLM/glm.hpp"

#include "PxPhysicsAPI.h"
#include "component.hpp"
#include "material.hpp"
#include "shader.hpp"
#include "debugrenderer.hpp"
#include "rigidbody.hpp"
#include "meshrenderer.hpp"
//#include <gamecamera.hpp>

using namespace physx;
using namespace Material;

class Collider : public Component {

public:

	PxShape* shape;
	PhysicMaterialFile* pmat;

	Collider();

	virtual void init(Editor* editor);

	virtual void updatePoseGeometry();

	virtual void updatePoseGeometryAndRelease();
};

class MeshCollider : public Collider {

public:

	MeshCollider();

	void init(Editor* editor, Rigidbody* rb, MeshRenderer* mr, bool convex);

	void updatePoseGeometry();

	void updatePoseGeometryAndRelease();

	PxTriangleMesh* createTriangleMesh(MeshRenderer* mRenderer, PxPhysics* gPhysics, PxCooking* gCooking);

	PxConvexMesh* createConvexMesh(MeshRenderer* mRenderer, PxPhysics* gPhysics, PxCooking* gCooking);
};

class BoxCollider : public Collider {

public:

	glm::vec3 center;
	glm::vec3 size;

	BoxCollider();

	void init(Editor* editor);

	void updatePoseGeometry();

	void updatePoseGeometryAndRelease();
};

class SphereCollider : public Collider {

public:

	glm::vec3 center;
	float radius = 0.5f;

	SphereCollider();

	void init(Editor* editor);

	void updatePoseGeometry();

	void updatePoseGeometryAndRelease();
};

class CapsuleCollider : public Collider {

public:

	glm::vec3 center;
	float radius = 0.5f;
	float height = 2.f;
	glm::ivec3 axis;

	CapsuleCollider();

	void init(Editor* editor);

	void updatePoseGeometry();

	void updatePoseGeometryAndRelease();

	int getAxis();

	void setAxis(int axis);

	glm::ivec3 getRotationVector(int axis);
};