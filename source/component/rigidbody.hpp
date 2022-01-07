#pragma once

#include "physics.hpp"
#include "component.hpp"

using namespace physx;

class Rigidbody : public Component {

public:

	PxRigidDynamic* body;

	Rigidbody();

	void updateGlobalPose();
};