#pragma once

#include "GLM/glm.hpp"

class MeshCollider {

public:

	unsigned int entID;
	bool convex = false;
	bool trigger = false;

	MeshCollider();
};

class BoxCollider {

public:

	unsigned int entID;
	bool trigger = false;
	glm::vec3 center;
	glm::vec3 size;

	BoxCollider();
};

class SphereCollider {

public:

	unsigned int entID;
	bool trigger = false;
	glm::vec3 center;
	float radius = 0.5f;

	SphereCollider();
};

class CapsuleCollider {

public:

	unsigned int entID;
	bool trigger = false;
	glm::vec3 center;
	float radius = 0.5f;
	float height = 1.f;

	CapsuleCollider();
};