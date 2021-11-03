#pragma once

#include <glm/glm.hpp>

#include "mesh.hpp"

enum class ComponentType{Light, MeshRenderer, RigidBody, Collider, Transform, Script, Animation, Animator};



struct AnimationComponent {

	int entID;
};

struct AnimatorComponent {

	int entID;
};

struct ColliderComponent {

	int entID;
};

//struct LightComponent {
//
//	int entID;
//	LightType type;
//	float power;
//	glm::vec3 color;
//};

struct MeshRendererComponent {

	int entID;
	unsigned int VAO;
	unsigned int indiceCount;
	unsigned int programID;
};

struct RigidBodyComponent {

	int entID;
};

struct ScriptComponent {

	int entID;
};

// unnecessary for now...
struct TransformComponent {

	int entID;
};