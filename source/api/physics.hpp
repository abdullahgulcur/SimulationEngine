#pragma once

#include <vector>

#include "PxPhysicsAPI.h"

#include "meshrenderer.hpp"
#include "transform.hpp"
#include "rigidbody.hpp"

using namespace physx;

class Rigidbody;

class Physics {

public:

	PxDefaultAllocator gAllocator;
	PxDefaultErrorCallback gErrorCallback;

	PxFoundation* gFoundation = NULL;
	PxPhysics* gPhysics = NULL;

	PxDefaultCpuDispatcher* gDispatcher = NULL;
	PxScene* gScene = NULL;
	PxMaterial* gMaterial = NULL;
	PxCooking* gCooking;
	//PxPvd* gPvd = NULL;

	PxReal stackZ = 10.0f;

	Physics();

	void initPhysics();

	void addConvexMesh(MeshRenderer* mRenderer, Transform* transform, Rigidbody* rb);

};