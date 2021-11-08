#include "transform.hpp"

Transform::Transform() {

	this->parent = NULL;
	id = 0;

	model = glm::mat4(1.0);
	localScale = glm::vec3(1, 1, 1);
}

Transform::Transform(Transform* parent, unsigned int id) {

	this->parent = parent;
	this->id = id;
	(this->parent->children).push_back(this);

	localScale = glm::vec3(1, 1, 1);
	model = glm::mat4(1.0);
}

Transform::Transform(Transform* parent, Transform* base, unsigned int id) {

	localPosition = base->localPosition;
	localRotation = base->localRotation;
	localScale = base->localScale;

	this->parent = parent;
	this->id = id;
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

	const glm::mat4 roationMatrix = transformY * transformX * transformZ;

	return glm::translate(glm::mat4(1.0f), localPosition) *
		roationMatrix *
		glm::scale(glm::mat4(1.0f), localScale);
}

void Transform::updateSelfAndChildTransforms()
{
	//localPosition = glm::vec3(0, 0, 0);
	//localRotation = glm::vec3(0, 0, 0);
	//localScale = glm::vec3(1, 1, 1);


	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(model, scale, rotation, translation, skew, perspective);

	glm::vec3 scale_parent;
	glm::quat rotation_parent;
	glm::vec3 translation_parent;
	glm::vec3 skew_parent;
	glm::vec4 perspective_parent;
	glm::decompose(parent->model, scale_parent, rotation_parent, translation_parent, skew_parent, perspective_parent);

	//// buggy
	//localScale = scale / scale_parent;
	////localPosition = (translation - translation_parent) / parent->localScale;
	localPosition = translation - translation_parent;
	////
	//localRotation = glm::degrees(glm::eulerAngles(rotation)) - glm::degrees(glm::eulerAngles(rotation_parent));

	Transform::updateSelfAndChild();
}

void Transform::updateSelfAndChild()
{
	if (parent != NULL)
		model = parent->model * getLocalModelMatrix();

	Transform::updateSelfAndChildRecursively();
}

void Transform::updateSelfAndChildRecursively()
{
	for (Transform* transform : children) {

		transform->model = model * transform->getLocalModelMatrix();
		transform->updateSelfAndChildRecursively();
	}
}

glm::vec3 Transform::getWorldPosition() {

	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(model, scale, rotation, translation, skew, perspective);

	return translation;
}