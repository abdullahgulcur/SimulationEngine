#pragma once

#include <glm/glm.hpp>

#include "component.hpp"

enum LightType { PointLight, DirectionalLight };

class Light : public Component {

private:

public:

	LightType lightType;
	float power = 40.f;
	glm::vec3 color;

	Light();
};