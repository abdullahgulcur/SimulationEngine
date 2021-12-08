#include "light.hpp"

Light::Light() {

	lightType = LightType::PointLight;
	color = glm::vec3(1, 1, 1);
}