#pragma once

class PhysicsComponent {

public:

	unsigned int entID;
	float mass = 1;
	bool useGravity = false;


	PhysicsComponent();
};