#include "stdafx.h"
#include "ObjectPhysicalInstance.h"
#include "MoteurWindows.h"

ObjectPhysicalInstance::ObjectPhysicalInstance()
{
	gPhysics = nullptr;
	geometry = nullptr;
	gMaterial = nullptr;
	rigidBody = nullptr;
}

ObjectPhysicalInstance::ObjectPhysicalInstance(PhysicalInitData& initData)
{
	isVehicule = false;
	isPhysical = true;
	gPhysics = CMoteurWindows::GetInstance().getPxPhysics();
	const TypeGeo type = initData.typeGeo;
	InitGeo(type, initData);
	gMaterial = gPhysics->createMaterial(initData.matStaticFriction, initData.matDynamicFriction, initData.matRestitution);
	InitRigidActor(type, initData);
	CMoteurWindows::GetInstance().getPxScene()->addActor(*rigidBody);
	//assign physical representation to the object logical representation aka game object
	assignToObject(static_cast<void*>(this));
}

void ObjectPhysicalInstance::InitGeo(TypeGeo type, PhysicalInitData& initData)
{
	switch (type) {
	case TypeGeo::eSPHERE: {
		geometry = new PxSphereGeometry(initData.attributesGeo.x);
		break;
	}
	case TypeGeo::ePLANE: {
		geometry = new PxPlaneGeometry();
		break;
	}
	case TypeGeo::eCAPSULE: {
		geometry = new PxCapsuleGeometry(initData.attributesGeo.x, initData.attributesGeo.y);
		break;
	}
	case TypeGeo::eBOX:
	case TypeGeo::eDEADZONE:
	case TypeGeo::eMONSTRE: {
		geometry = new PxBoxGeometry(initData.attributesGeo.x, initData.attributesGeo.y, initData.attributesGeo.z);
		break;
	}

	case TypeGeo::eTRIANGLEMESH: {
		geometry = new PxTriangleMeshGeometry(setupMeshColision(initData.meshInstanceName, initData));
		break;
	}
	case TypeGeo::eCONVEXMESH: {
		CreateWheelMesh(initData);
	}
	}
}

void ObjectPhysicalInstance::InitRigidActor(TypeGeo type, PhysicalInitData& initData)
{
	PxShape* shape = gPhysics->createShape(*geometry, *gMaterial);
	if (initData.isDynamic)
	{
		CreateDynamic(type,initData,shape);
	}
	else
	{

		CreateStatic(type, initData, shape);
	}
}


void ObjectPhysicalInstance::CreateDynamic(TypeGeo type, PhysicalInitData& initData, PxShape* shape)
{
	PxRigidDynamic* dynamicActor = gPhysics->createRigidDynamic(initData.transform);

	if (type == TypeGeo::eMONSTRE)
	{
		dynamicActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
		shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
		shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	}

	dynamicActor->setWakeCounter(10000);
	dynamicActor->attachShape(*shape);
	dynamicActor->setRigidDynamicLockFlags(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X | PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z);

	rigidBody = static_cast<PxRigidActor*>(dynamicActor);
}

void ObjectPhysicalInstance::CreateStatic(TypeGeo type, PhysicalInitData& initData, PxShape* shape)
{
	PxRigidStatic* staticActor = gPhysics->createRigidStatic(initData.transform);
	if (type == TypeGeo::eDEADZONE)
	{
		shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
		shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	}

	staticActor->attachShape(*shape);

	rigidBody = staticActor;
}

void ObjectPhysicalInstance::CreateWheelMesh(PhysicalInitData& initData)
{
	isVehicule = true;
	PxVec3 points[3 * 32];
	for (PxU32 i = 0; i < 32; i++)
	{
		const PxF32 cosTheta = PxCos(i * PxPi * 2.0f / 32.0f);
		const PxF32 sinTheta = PxSin(i * PxPi * 2.0f / 32.0f);
		const PxF32 y = initData.attributesGeo.x * cosTheta;
		const PxF32 z = initData.attributesGeo.x * sinTheta;
		const PxF32 yd = (initData.attributesGeo.x - 0.25f) * cosTheta;
		const PxF32 zd = (initData.attributesGeo.x - 0.25f) * sinTheta;
		points[3 * i + 0] = PxVec3(yd, zd, -initData.attributesGeo.y / 2.0f);
		points[3 * i + 1] = PxVec3(y, z, 0);
		points[3 * i + 2] = PxVec3(yd, zd, +initData.attributesGeo.y / 2.0f);
	}

	geometry = new PxConvexMeshGeometry(setupConvexMeshColision(points, 96));
}

void ObjectPhysicalInstance::setGlobalPose(const PxTransform& pose, bool autowake)
{
	rigidBody->setGlobalPose(pose, autowake);
}

void ObjectPhysicalInstance::setMass(PxReal mass)
{
	static_cast<PxRigidDynamic*>(rigidBody)->setMass(mass);
}

void ObjectPhysicalInstance::setMassSpaceInertiaTensor(const PxVec3& m)
{
	static_cast<PxRigidDynamic*>(rigidBody)->setMassSpaceInertiaTensor(m);
}

void ObjectPhysicalInstance::setLinearDamping(PxReal linDamp)
{
	static_cast<PxRigidDynamic*>(rigidBody)->setLinearDamping(linDamp);
}

void ObjectPhysicalInstance::setAngularDamping(PxReal angDamp)
{
	static_cast<PxRigidDynamic*>(rigidBody)->setAngularDamping(angDamp);
}

void ObjectPhysicalInstance::setLinearVelocity(const PxVec3& linVel, bool autowake)
{
	static_cast<PxRigidDynamic*>(rigidBody)->setLinearVelocity(linVel, autowake);
}

void ObjectPhysicalInstance::setAngularVelocity(const PxVec3& angVel, bool autowake)
{
	static_cast<PxRigidDynamic*>(rigidBody)->setAngularVelocity(angVel, autowake);
}

void ObjectPhysicalInstance::addForce(const PxVec3& force, PxForceMode::Enum mode, bool autowake)
{
	static_cast<PxRigidDynamic*>(rigidBody)->addForce(force, mode, autowake);
}

void ObjectPhysicalInstance::addTorque(const PxVec3& torque, PxForceMode::Enum mode, bool autowake)
{
	static_cast<PxRigidDynamic*>(rigidBody)->addTorque(torque, mode, autowake);
}

void ObjectPhysicalInstance::clearForce(PxForceMode::Enum mode)
{
	static_cast<PxRigidDynamic*>(rigidBody)->clearForce(mode);
}

void ObjectPhysicalInstance::clearTorque(PxForceMode::Enum mode)
{
	static_cast<PxRigidDynamic*>(rigidBody)->clearTorque(mode);
}

PxTransform ObjectPhysicalInstance::getGlobalPose()
{
	return rigidBody->getGlobalPose();
}

PxReal ObjectPhysicalInstance::getMass()
{
	return static_cast<PxRigidDynamic*>(rigidBody)->getMass();
}

PxVec3 ObjectPhysicalInstance::getMassSpaceInertiaTensor()
{
	return static_cast<PxRigidDynamic*>(rigidBody)->getMassSpaceInertiaTensor();
}

PxReal ObjectPhysicalInstance::getLinearDamping()
{
	return static_cast<PxRigidDynamic*>(rigidBody)->getLinearDamping();
}

PxReal ObjectPhysicalInstance::getAngularDamping()
{
	return static_cast<PxRigidDynamic*>(rigidBody)->getAngularDamping();
}

PxVec3 ObjectPhysicalInstance::getLinearVelocity()
{
	return static_cast<PxRigidDynamic*>(rigidBody)->getLinearVelocity();
}

PxVec3 ObjectPhysicalInstance::getAngularVelocity()
{
	return static_cast<PxRigidDynamic*>(rigidBody)->getAngularVelocity();
}

void ObjectPhysicalInstance::assignToObject(void* object) {
	rigidBody->userData = object;
}

PxTriangleMesh* ObjectPhysicalInstance::setupMeshColision(wstring meshInstanceName, PhysicalInitData& initData)
{
	auto pxPhys = gPhysics;
	auto pxCooking = CMoteurWindows::GetInstance().getPxCooking();
	CMeshInstance* pMesh = CMoteurWindows::GetInstance().GetMeshInstanceManager().GetInstance(meshInstanceName);
	size_t vertexNb = pMesh->getVerticesSize();
	size_t indicesSize = pMesh->getIndicesSize();
	CSommetTerrain* pVertices;
	pMesh->getVertices(pVertices);
	uint32_t* pIndices;
	pMesh->getIndices(pIndices);

	physx::PxVec3* pPxVerts = new physx::PxVec3[vertexNb];

	for (int i = 0; i < vertexNb; i++)
	{
		pPxVerts[i] = PxVec3(pVertices[i].getPosition().x * initData.attributesGeo.x, pVertices[i].getPosition().y * initData.attributesGeo.y, pVertices[i].getPosition().z * initData.attributesGeo.z);
	}

	physx::PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = static_cast<physx::PxU32>(vertexNb);
	meshDesc.points.stride = sizeof(physx::PxVec3);
	meshDesc.points.data = pPxVerts;

	meshDesc.triangles.count = static_cast<physx::PxU32>(indicesSize / 3);
	meshDesc.triangles.stride = 3 * sizeof(physx::PxU32);
	meshDesc.triangles.data = pIndices;

	physx::PxDefaultMemoryOutputStream writeBuffer;
	bool status = pxCooking->cookTriangleMesh(meshDesc, writeBuffer);
	if (!status)
	{
		delete[] pPxVerts;
		return nullptr;
	}

	physx::PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	physx::PxTriangleMesh* triangleMesh = static_cast<physx::PxTriangleMesh*>(pxPhys->createTriangleMesh(readBuffer));
	if (!triangleMesh)
	{
		delete[] pPxVerts;
		return nullptr;
	}
	return triangleMesh;
	return nullptr;
}

PxConvexMesh* ObjectPhysicalInstance::setupConvexMeshColision(const PxVec3* verts, const PxU32 numVerts)
{
	auto pxPhys = gPhysics;
	auto pxCooking = CMoteurWindows::GetInstance().getPxCooking();
	

	// Create descriptor for convex mesh
	PxConvexMeshDesc convexDesc;
	convexDesc.points.count = numVerts;
	convexDesc.points.stride = sizeof(PxVec3);
	convexDesc.points.data = verts;
	convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

	PxConvexMesh* convexMesh = NULL;
	PxDefaultMemoryOutputStream buf;
	if (pxCooking->cookConvexMesh(convexDesc, buf))
	{
		PxDefaultMemoryInputData id(buf.getData(), buf.getSize());
		convexMesh = pxPhys->createConvexMesh(id);
	}
	return convexMesh;
}

PxRigidActor* ObjectPhysicalInstance::getRigidBody()
{
	return rigidBody;
}

PxPhysics* ObjectPhysicalInstance::getPhysics()
{
	return gPhysics;
}

PxMaterial* ObjectPhysicalInstance::getMaterial()
{
	return gMaterial;
}

PxGeometry* ObjectPhysicalInstance::getGeometry()
{
	return geometry;
}

bool ObjectPhysicalInstance::getIsVehicule()
{
	return isVehicule;
}

void ObjectPhysicalInstance::setRigidBody(PxRigidActor& _rigidActor)
{
	rigidBody = &_rigidActor;
}

void ObjectPhysicalInstance::setPhysics(PxPhysics& _gPhysics)
{
	gPhysics = &_gPhysics;
}

void ObjectPhysicalInstance::setMaterial(PxMaterial& _gMaterial)
{
	gMaterial = &_gMaterial;
}

void ObjectPhysicalInstance::setGeometry(PxGeometry& _geometry)
{
	geometry = &_geometry;
}
