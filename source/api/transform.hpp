#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <math.hpp>

class Transform {

private:


public:

	unsigned int id;
	Transform* parent;
	std::vector<Transform*> children;

	//glm::vec3 position;
	//glm::vec3 rotation;
	//glm::vec3 scale;

	glm::vec3 localPosition;
	glm::vec3 localRotation;
	glm::vec3 localScale;

	glm::mat4 model;

	Transform();

	Transform(Transform* parent, unsigned int id);

	Transform(Transform* parent, Transform* base, unsigned int id);

	glm::mat4 getLocalModelMatrix();

	void updateSelfAndChildTransforms(int type);

	void updateSelfAndChild();

	void updateSelfAndChildRecursively();

	glm::vec3 getWorldPosition();
};