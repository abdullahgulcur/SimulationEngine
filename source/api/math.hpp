#pragma once

#include<GLM/glm.hpp>

namespace Math {

	bool decompose(glm::mat4& ModelMatrix, glm::vec3& Scale, glm::vec3& Orientation, glm::vec3& Translation);

	bool decompose(glm::mat4& ModelMatrix, glm::vec3& Translation, glm::vec3& Scale);

	float getFOV_X(float fovY, float aspectRatio);

	template <typename T>
	T getMax(T x, T y) {

		return x > y ? x : y;
	}

	template <typename T>
	T getMin(T x, T y) {

		return x < y ? x : y;
	}
}