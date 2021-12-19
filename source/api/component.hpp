#pragma once

#include "transform.hpp"

enum class ComponentType { Light, MeshRenderer, Rigidbody, MeshCollider, BoxCollider, SphereCollider, CapsuleCollider, Transform, Script, Animation, Animator };

class Component {

private:

public:

	Transform* transform;

	Component();

	virtual ~Component();
};