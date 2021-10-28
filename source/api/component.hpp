#pragma once

#include <glm/glm.hpp>

#include "mesh_renderer.hpp"

enum class ComponentType{Light, MeshRenderer, RigidBody, Collider, Transform, Script, Animation, Animator};

enum LightType{ PointLight, DirectionalLight };

/*
* All these id's transform's id's actually...
*/

struct AnimationComponent {

	int id;
};

struct AnimatorComponent {

	int id;
};

struct ColliderComponent {

	int id;
};

struct LightComponent {

	int id;
	LightType type;
	float power;
	glm::vec3 color;
};

struct MeshRendererComponent {

	int id;
	std::string meshName;
	Transform* transform;
	MeshRenderer* renderer;
};

struct RigidBodyComponent {

	int id;
};

struct ScriptComponent {

	int id;
};

// unnecessary for now...
struct TransformComponent {

	int id;
};