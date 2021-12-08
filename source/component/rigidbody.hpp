#pragma once

#include "physics.hpp"
#include "component.hpp"

using namespace physx;

class Rigidbody : public Component {

public:

	float mass = 1;

	bool useGravity = false;
	bool isKinematic = false;

	glm::vec3 freezePos;
	glm::vec3 freezeRot;

	PxRigidDynamic* body;

	Rigidbody();
};