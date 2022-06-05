#include "physics.hpp"

Physics::Physics() {

	Physics::initPhysics();

}

void Physics::addConvexMesh(MeshRenderer* mRenderer, Transform* transform, Rigidbody* rb) {

	//int vert_size = mRenderer->mesh->vertices.size();
	//int tri_size = mRenderer->mesh->indices.size();
	//float* verts = new float[vert_size * 3];
	//float* triangles = new float[tri_size];

	//for (int i = 0; i < vert_size; i++) {
	//	verts[i * 3 + 0] = mRenderer->mesh->vertices[i].position.x;
	//	verts[i * 3 + 1] = mRenderer->mesh->vertices[i].position.y;
	//	verts[i * 3 + 2] = mRenderer->mesh->vertices[i].position.z;
	//}

	//for (int i = 0; i < tri_size; i++)
	//	triangles[i] = mRenderer->mesh->indices[i];

	//PxTriangleMeshDesc meshDesc;
	//meshDesc.points.count = vert_size * 3;
	//meshDesc.points.stride = sizeof(float) * 3;
	//meshDesc.triangles.count = tri_size / 3;
	//meshDesc.points.stride = sizeof(float) * 3;
	//meshDesc.points.data = verts;
	//meshDesc.triangles.data = triangles;

	//PxTriangleMesh* aTriangleMesh = gCooking->createTriangleMesh(meshDesc, gPhysics->getPhysicsInsertionCallback());
	// 
	//int counter = 0;
	//for (auto& it : mRenderer->mesh->vertices) {

	//	convexVerts[counter] = PxVec3(it.position.x, it.position.y, it.position.z);
	//	counter++;
	//}
	//meshDesc.points.data = convexVerts;

	//PxU32* indices = new PxU32[mRenderer->mesh->indices.size()];

	//counter = 0;
	//for (auto& it : mRenderer->mesh->indices) {

	//	indices[counter] = it;
	//	counter++;
	//}

	//meshDesc.triangles.count = mRenderer->mesh->indices.size() / 3;
	//meshDesc.triangles.stride = 3 * sizeof(PxU32);
	//meshDesc.triangles.data = indices;

	//PxTriangleMesh* aTriangleMesh = gCooking->createTriangleMesh(meshDesc,
	//	gPhysics->getPhysicsInsertionCallback());

	//PxShape* shape = gPhysics->createShape(PxTriangleMeshGeometry(aTriangleMesh), *gMaterial);
	////shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	//shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);





	/*PxVec3* convexVerts = new PxVec3[mRenderer->mesh->vertices.size()];

	int counter = 0;
	for (auto& it : mRenderer->mesh->vertices) {

		convexVerts[counter] = PxVec3(it.position.x, it.position.y, it.position.z);
		counter++;
	}

	PxConvexMeshDesc convexDesc;
	convexDesc.points.count = 5;
	convexDesc.points.stride = sizeof(PxVec3);
	convexDesc.points.data = convexVerts;
	convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

	PxConvexMesh* aConvexMesh = gCooking->createConvexMesh(convexDesc, gPhysics->getPhysicsInsertionCallback());

	PxTransform tm(PxVec3(transform->localPosition.x, transform->localPosition.y, transform->localPosition.z));
	PxRigidDynamic* body = gPhysics->createRigidDynamic(tm);
	PxShape* shape = gPhysics->createShape(PxConvexMeshGeometry(aConvexMesh), *gMaterial);
	body->attachShape(*shape);
	PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
	gScene->addActor(*body);

	shape->release();*/

	PxShape* shape = gPhysics->createShape(PxBoxGeometry(1, 1, 1), *gMaterial);

	glm::quat myquaternion = glm::quat(transform->globalRotation);
	PxTransform tm(PxVec3(transform->globalPosition.x, transform->globalPosition.y, transform->globalPosition.z),
		PxQuat(myquaternion.x, myquaternion.y, myquaternion.z, myquaternion.w));

	PxRigidDynamic* body = gPhysics->createRigidDynamic(tm);
	body->attachShape(*shape);
	PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
	gScene->addActor(*body);
	shape->release();

	rb->body = body;
}

//void Physics::enableCollider() {
//
//}

void Physics::initPhysics()
{
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true);

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	gScene = gPhysics->createScene(sceneDesc);
	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	
	PxTolerancesScale scale;
	PxCookingParams params(scale);
	params.meshWeldTolerance = 0.001f;
	params.meshPreprocessParams = PxMeshPreprocessingFlags(PxMeshPreprocessingFlag::eWELD_VERTICES);
	params.buildGPUData = true; //Enable GRB data being produced in cooking.
	gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, params);

	//PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 0), *gMaterial);
	//gScene->addActor(*groundPlane);
}