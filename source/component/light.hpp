#pragma once

#include <glm/glm.hpp>

enum LightType { PointLight, DirectionalLight };

class Light {

private:

public:

	int entID;
	LightType type;
	float power;
	glm::vec3 color;

	Light();
};