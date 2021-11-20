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

	//const glm::mat4 roationMatrix = transformY * transformX * transformZ;
	const glm::mat4 roationMatrix = transformZ * transformY * transformX;

	return glm::translate(glm::mat4(1.0f), localPosition) *
		roationMatrix *
		glm::scale(glm::mat4(1.0f), localScale);
}

void Transform::updateSelfAndChildTransforms(int type)
{
	glm::mat4 localTransform = glm::inverse(parent->model) * model;

	glm::vec3 scale;
	glm::vec3 rotation;
	glm::vec3 position;
	Math::decompose(localTransform, scale, rotation, position);
	
	localScale = scale;
	localRotation = rotation * (180.f / glm::pi<float>());
	localPosition = position;

	Transform::updateSelfAndChildRecursively();
}

void Transform::updateSelfAndChild()
{
	if (parent != NULL) {

		model = parent->model * getLocalModelMatrix();
	}

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