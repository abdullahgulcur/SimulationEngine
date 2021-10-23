#pragma once

#include <string>

class Scene;
class Entity;

enum class EntityType { sceneObj, pointLight, directionalLight };

class SceneObject {

private:

public:

	std::string name;
	EntityType type;
	Entity* addr;

	SceneObject();

};