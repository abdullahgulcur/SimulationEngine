#include "collider.hpp"
#include "entity.hpp"
#include "meshrenderer.hpp"
#include "editor.hpp"

Collider::Collider() {

}

void Collider::init(Editor* editor) {

}

void Collider::updatePoseGeometry() {

}

void Collider::updatePoseGeometryAndRelease() {

}

BoxCollider::BoxCollider() {

	center = glm::vec3(0.f, 0.f, 0.f);
	size = glm::vec3(1.f, 1.f, 1.f);
}

void BoxCollider::init(Editor* editor) {

	pmat = &editor->fileSystem->physicmaterials["Default"];
	pmat->colliderCompAddrs.push_back(this);
	glm::vec3 size = transform->globalScale * this->size / 2.f;
	shape = editor->physics->gPhysics->createShape(PxBoxGeometry(size.x, size.y, size.z), *pmat->pxmat, true);
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
	glm::vec3 center = transform->globalScale * center;
	shape->setLocalPose(PxTransform(center.x, center.y, center.z));

	if (Rigidbody* rb = transform->entity->getComponent<Rigidbody>()) {
		rb->body->attachShape(*shape);
		shape->release();
	}
}

// todo: only do when collider props or transform's scale is changed 
void BoxCollider::updatePoseGeometry() {

	glm::vec3 size = transform->globalScale * this->size / 2.f;
	glm::vec3 center = transform->globalScale * this->center;
	shape->setLocalPose(PxTransform(center.x, center.y, center.z));
	shape->setGeometry(PxBoxGeometry(size.x, size.y, size.z));
}

void BoxCollider::updatePoseGeometryAndRelease() {

	glm::vec3 size = transform->globalScale * this->size / 2.f;
	glm::vec3 center = transform->globalScale * this->center;
	shape->setLocalPose(PxTransform(center.x, center.y, center.z));
	shape->setGeometry(PxBoxGeometry(size.x, size.y, size.z));
	//shape->release();
}

// update only pose !!!

SphereCollider::SphereCollider() {

	center = glm::vec3(0.f, 0.f, 0.f);
}

void SphereCollider::init(Editor* editor) {

	pmat = &editor->fileSystem->physicmaterials["Default"];
	shape = editor->physics->gPhysics->createShape(PxSphereGeometry(0.5f), *pmat->pxmat, true);
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
	glm::vec3 center = transform->globalScale * center;
	shape->setLocalPose(PxTransform(center.x, center.y, center.z));

	if (Rigidbody* rb = transform->entity->getComponent<Rigidbody>()) {
		rb->body->attachShape(*shape);
		shape->release();
	}
}

void SphereCollider::updatePoseGeometry() {

	glm::vec3 center = transform->globalScale * this->center;
	shape->setLocalPose(PxTransform(center.x, center.y, center.z));
	float max = transform->globalScale.x > transform->globalScale.y ? transform->globalScale.x : transform->globalScale.y;
	max = max > transform->globalScale.z ? max : transform->globalScale.z;
	shape->setGeometry(PxSphereGeometry(radius * max));
}

void SphereCollider::updatePoseGeometryAndRelease() {

	glm::vec3 center = transform->globalScale * this->center;
	shape->setLocalPose(PxTransform(center.x, center.y, center.z));
	float max = transform->globalScale.x > transform->globalScale.y ? transform->globalScale.x : transform->globalScale.y;
	max = max > transform->globalScale.z ? max : transform->globalScale.z;
	shape->setGeometry(PxSphereGeometry(radius * max));
	//shape->release();
}

CapsuleCollider::CapsuleCollider() {

	center = glm::vec3(0.f, 0.f, 0.f);
	axis = glm::ivec3(0, 1, 0);
}

void CapsuleCollider::init(Editor* editor) {

	pmat = &editor->fileSystem->physicmaterials["Default"];
	shape = editor->physics->gPhysics->createShape(PxCapsuleGeometry(radius, height / 2), *pmat->pxmat, true);
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
	PxTransform relativePose(PxVec3(center.x, center.y, center.z), PxQuat(PxHalfPi, PxVec3(0, 0, 1)));
	shape->setLocalPose(relativePose);

	if (Rigidbody* rb = transform->entity->getComponent<Rigidbody>()) {
		rb->body->attachShape(*shape);
		shape->release();
	}
}

void CapsuleCollider::updatePoseGeometry() {

	glm::ivec3 rotVec = getRotationVector(getAxis());
	PxTransform pose(PxVec3(center.x, center.y, center.z), PxQuat(PxHalfPi, PxVec3(rotVec.x, rotVec.y, rotVec.z)));
	shape->setLocalPose(pose);
	shape->setGeometry(PxCapsuleGeometry(radius, height / 2));
}

void CapsuleCollider::updatePoseGeometryAndRelease() {

}

int CapsuleCollider::getAxis() {

	if (axis.x == 1)
		return 0;
	else if (axis.y == 1)
		return 1;
	else if (axis.z == 1)
		return 2;
}

void CapsuleCollider::setAxis(int axis) {

	if (axis == 0)
		this->axis = glm::ivec3(1, 0, 0);
	else if (axis == 1)
		this->axis = glm::ivec3(0, 1, 0);
	else if (axis == 1)
		this->axis = glm::ivec3(0, 0, 1);
}

glm::ivec3 CapsuleCollider::getRotationVector(int axis) {

	if (axis == 0)
		return glm::ivec3(1, 0, 0);
	else if (axis == 1)
		return glm::ivec3(0, 0, 1);
	else if (axis == 1)
		return glm::ivec3(0, 1, 0);
}

MeshCollider::MeshCollider() {

}

void MeshCollider::init(Editor* editor, Rigidbody* rb, MeshRenderer* mr, bool convex) {

	pmat = &editor->fileSystem->physicmaterials["Default"];
	pmat->colliderCompAddrs.push_back(this);

	if (convex) {

		PxConvexMesh* convexMesh = createConvexMesh(mr, editor->physics->gPhysics, editor->physics->gCooking);
		shape = editor->physics->gPhysics->createShape(PxConvexMeshGeometry(convexMesh,
			PxVec3(transform->globalScale.x, transform->globalScale.y,
				transform->globalScale.z)), *pmat->pxmat, true);
		shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);

		if (rb) {
			rb->body->attachShape(*shape);
			shape->release();
		}
	}
	else {

		PxTriangleMesh* triangleMesh = createTriangleMesh(mr, editor->physics->gPhysics, editor->physics->gCooking);
		shape = editor->physics->gPhysics->createShape(PxTriangleMeshGeometry(triangleMesh,
			PxVec3(transform->globalScale.x, transform->globalScale.y,
				transform->globalScale.z)), *pmat->pxmat, true);
		shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);

		if (rb) {
			rb->body->attachShape(*shape);
			shape->release();
		}
	}
	
}

void MeshCollider::updatePoseGeometry() {

	if (shape->getGeometryType() & PxGeometryType::eCONVEXMESH) {

		PxConvexMeshGeometry convexMesh;
		shape->getConvexMeshGeometry(convexMesh);
		convexMesh.scale = PxVec3(transform->globalScale.x, transform->globalScale.y, transform->globalScale.z);
		shape->setGeometry(convexMesh);
	}
	else {

		PxTriangleMeshGeometry triangleMesh;
		shape->getTriangleMeshGeometry(triangleMesh);
		triangleMesh.scale = PxVec3(transform->globalScale.x, transform->globalScale.y, transform->globalScale.z);
		shape->setGeometry(triangleMesh);
	}
}

void MeshCollider::updatePoseGeometryAndRelease() {

	if (shape->getGeometryType() & PxGeometryType::eCONVEXMESH) {

		PxConvexMeshGeometry convexMesh;
		shape->getConvexMeshGeometry(convexMesh);
		convexMesh.scale = PxVec3(transform->globalScale.x, transform->globalScale.y, transform->globalScale.z);
		shape->setGeometry(convexMesh);
	}
	else {

		PxTriangleMeshGeometry triangleMesh;
		shape->getTriangleMeshGeometry(triangleMesh);
		triangleMesh.scale = PxVec3(transform->globalScale.x, transform->globalScale.y, transform->globalScale.z);
		shape->setGeometry(triangleMesh);
	}

	//shape->release();
}

PxTriangleMesh* MeshCollider::createTriangleMesh(MeshRenderer* mRenderer, PxPhysics* gPhysics, PxCooking* gCooking) {

	unsigned int vert_size = mRenderer->mesh->vertices.size();
	unsigned int tri_size = mRenderer->mesh->indices.size() / 3;
	float* verts = new float[vert_size * 3];
	unsigned int* indices = new unsigned int[tri_size * 3];

	for (int i = 0; i < vert_size; i++) {
		verts[i * 3 + 0] = mRenderer->mesh->vertices[i].position.x;
		verts[i * 3 + 1] = mRenderer->mesh->vertices[i].position.y;
		verts[i * 3 + 2] = mRenderer->mesh->vertices[i].position.z;
	}

	for (int i = 0; i < tri_size; i++)
		indices[i] = mRenderer->mesh->indices[i];

	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = vert_size;
	meshDesc.points.stride = sizeof(float) * 3;
	meshDesc.triangles.count = tri_size;
	meshDesc.triangles.stride = sizeof(unsigned int) * 3;
	meshDesc.points.data = verts;
	meshDesc.triangles.data = indices;

	return gCooking->createTriangleMesh(meshDesc, gPhysics->getPhysicsInsertionCallback());
}

PxConvexMesh* MeshCollider::createConvexMesh(MeshRenderer* mRenderer, PxPhysics* gPhysics, PxCooking* gCooking) {

	unsigned int vert_size = mRenderer->mesh->vertices.size();
	float* verts = new float[vert_size * 3];

	for (int i = 0; i < vert_size; i++) {
		verts[i * 3 + 0] = mRenderer->mesh->vertices[i].position.x;
		verts[i * 3 + 1] = mRenderer->mesh->vertices[i].position.y;
		verts[i * 3 + 2] = mRenderer->mesh->vertices[i].position.z;
	}

	PxConvexMeshDesc meshDesc;
	meshDesc.points.count = vert_size;
	meshDesc.points.stride = sizeof(float) * 3;
	meshDesc.points.data = verts;
	meshDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX | PxConvexFlag::eDISABLE_MESH_VALIDATION | PxConvexFlag::eFAST_INERTIA_COMPUTATION;

	return gCooking->createConvexMesh(meshDesc, gPhysics->getPhysicsInsertionCallback());
}