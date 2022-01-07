#include "rigidbody.hpp"

Rigidbody::Rigidbody() {}

void Rigidbody::updateGlobalPose() {

	glm::quat myquaternion = glm::quat(transform->globalRotation);
	PxTransform tm(PxVec3(transform->globalPosition.x, transform->globalPosition.y, transform->globalPosition.z),
		PxQuat(myquaternion.x, myquaternion.y, myquaternion.z, myquaternion.w));

	body->setGlobalPose(tm);
	//body->release();
}