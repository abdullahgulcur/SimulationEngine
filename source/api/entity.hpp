#pragma once

#include <string>

#include "transform.hpp"

enum class EntityType { sceneObj, pointLight, directionalLight };

class Entity {

private:

public:

	std::string name;
	EntityType type;
	Transform* transform;

	Entity();

};