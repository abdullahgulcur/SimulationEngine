#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <math.hpp>

class Entity;

class Transform {

private:


public:

	unsigned int id;
	Transform* parent;
	Entity* entity;
	std::vector<Transform*> children;

	glm::vec3 localPosition;
	glm::vec3 localRotation;
	glm::vec3 localScale;

	glm::vec3 globalPosition;
	glm::vec3 globalRotation;
	glm::vec3 globalScale;

	glm::mat4 model;

	Transform(Entity* ent);

	Transform(Entity* ent, Transform* parent, unsigned int id);

	Transform(Entity* ent, Transform* parent, Transform* base, unsigned int id);

	glm::mat4 getLocalModelMatrix();

	void updateSelfAndChildTransforms();

	void updateSelfAndChild();

	void updateSelfAndChildRecursively();

	void setGlobalTransformation();

};