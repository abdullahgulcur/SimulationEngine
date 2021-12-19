#pragma once

#include "GLM/glm.hpp"

#include "shader.hpp"

class ColliderRenderer {

private:

public:

	unsigned int count;
	unsigned int VAO;
	unsigned int shaderProgramID;
	glm::vec3 color;

	ColliderRenderer();

	virtual void drawCollider(glm::mat4 MVP);

};

class BoxColliderRenderer : public ColliderRenderer {

private:

public:

	BoxColliderRenderer();

};

class SphereColliderRenderer : public ColliderRenderer {

private:

public:

	SphereColliderRenderer();

	void drawCollider(glm::mat4 MVP);

};