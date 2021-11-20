#pragma once

#include<GLM/glm.hpp>

namespace Math {

	bool decompose(glm::mat4& ModelMatrix, glm::vec3& Scale, glm::vec3& Orientation, glm::vec3& Translation);

}