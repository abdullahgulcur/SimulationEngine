#include "rigidbody.hpp"

Rigidbody::Rigidbody() {

	//type = ComponentType::Rigidbody;

	freezePos = glm::vec3(false, false, false);
	freezeRot = glm::vec3(false, false, false);
}