#include "transform.hpp"
#include "entity.hpp"
#include "rigidbody.hpp"

Transform::Transform(Entity* ent) {

	this->entity = ent;
	this->parent = NULL;

	model = glm::mat4(1.0);
	localScale = glm::vec3(1, 1, 1);
	globalScale = glm::vec3(1, 1, 1);
}

Transform::Transform(Entity* ent, Transform* parent) {

	this->entity = ent;
	this->parent = parent;
	(this->parent->children).push_back(this);

	localPosition = glm::vec3(0, 0, 0);
	localRotation = glm::vec3(0, 0, 0);
	localScale = glm::vec3(1, 1, 1);

	globalPosition = parent->globalPosition;
	globalRotation = parent->globalRotation;
	globalScale = parent->globalScale;

	model = parent->model;
}

Transform::Transform(Entity* ent, Transform* parent, Transform* base) {

	this->entity = ent;
	localPosition = base->localPosition;
	localRotation = base->localRotation;
	localScale = base->localScale;

	globalPosition = base->globalPosition;
	globalRotation = base->globalRotation;
	globalScale = base->globalScale;

	this->parent = parent;
	(this->parent->children).push_back(this);
}

glm::mat4 Transform::getLocalModelMatrix()
{
	const glm::mat4 transformX = glm::rotate(glm::mat4(1.0f),
		glm::radians(localRotation.x),
		glm::vec3(1.0f, 0.0f, 0.0f));
	const glm::mat4 transformY = glm::rotate(glm::mat4(1.0f),
		glm::radians(localRotation.y),
		glm::vec3(0.0f, 1.0f, 0.0f));
	const glm::mat4 transformZ = glm::rotate(glm::mat4(1.0f),
		glm::radians(localRotation.z),
		glm::vec3(0.0f, 0.0f, 1.0f));

	const glm::mat4 roationMatrix = transformZ * transformY * transformX;

	return glm::translate(glm::mat4(1.0f), localPosition) *
		roationMatrix *
		glm::scale(glm::mat4(1.0f), localScale);
}

void Transform::updateSelfAndChildTransforms()
{
	Transform::setLocalTransformation();
	Transform::setGlobalTransformation();
	Transform::updatePhysics();

	Transform::updateSelfAndChildRecursively();
}

void Transform::updateSelfAndChild()
{
	if (parent != NULL) {

		model = parent->model * getLocalModelMatrix();
		Transform::setGlobalTransformation();
		Transform::updatePhysics();
	}
	Transform::updateSelfAndChildRecursively();
}

void Transform::updateSelfAndChildRecursively()
{
	for (Transform* transform : children) {

		transform->model = model * transform->getLocalModelMatrix();
		transform->setGlobalTransformation();
		Transform::updatePhysics();

		transform->updateSelfAndChildRecursively();
	}
}

void Transform::setLocalTransformation() {

	glm::mat4 localTransform = glm::inverse(parent->model) * model;

	glm::vec3 scale;
	glm::vec3 rotation;
	glm::vec3 position;
	Math::decompose(localTransform, scale, rotation, position);

	localScale = scale;
	localRotation = rotation * (180.f / glm::pi<float>());
	localPosition = position;
}

void Transform::setGlobalTransformation() {

	glm::vec3 scale;
	glm::vec3 rotation;
	glm::vec3 position;
	Math::decompose(model, scale, rotation, position);

	globalPosition = position;
	globalRotation = rotation;
	globalScale = scale;
}

void Transform::updatePhysics() {

	Rigidbody* rb = entity->getComponent<Rigidbody>();
	if (rb)
		rb->updateGlobalPose();

	for (auto& comp : entity->getComponents<Collider>()) {

		if (rb)
			comp->updatePoseGeometryAndRelease();
		else
			comp->updatePoseGeometry();
	}
}