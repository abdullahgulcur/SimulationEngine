#pragma once

enum class ComponentType { Light, MeshRenderer, Rigidbody, MeshCollider, BoxCollider, SphereCollider, CapsuleCollider, Transform, Script, Animation, Animator };

class Component {

private:

public:

	Component();

	virtual ~Component();
};