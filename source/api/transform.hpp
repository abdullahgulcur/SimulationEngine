#pragma once

#include <vector>

#include <glm/glm.hpp>


class Transform {

public:

	int id;
	Transform* parent;
	std::vector<Transform*> children;

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

};