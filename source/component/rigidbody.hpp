#pragma once

class Rigidbody {

public:

	unsigned int entID;
	float mass = 1;
	bool useGravity = false;

	Rigidbody();
};