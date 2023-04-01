#pragma once
#include <ctype.h>
#include "PxPhysicsAPI.h"
#include "PxSimulationEventCallback.h"
#include "ObjectPhysicalInitData.h"

using namespace physx;

class ObjectPhysicalInstance
{
protected:
	bool isPhysical = false;

private:
	PxRigidActor* rigidBody;
	PxPhysics* gPhysics;
	PxMaterial* gMaterial;
	PxGeometry* geometry;
	bool isVehicule;

	PxTriangleMesh* setupMeshColision(wstring meshInstanceName, PhysicalInitData& initData);
	PxConvexMesh* setupConvexMeshColision(const PxVec3* verts, const PxU32 numVerts);

	void InitGeo(TypeGeo type, PhysicalInitData& initData);
	void InitRigidActor(TypeGeo type, PhysicalInitData& initData);
	void CreateDynamic(TypeGeo type, PhysicalInitData& initData, PxShape* shape);
	void CreateStatic(TypeGeo type, PhysicalInitData& initData, PxShape* shape);
	void CreateWheelMesh(PhysicalInitData& initData);

public:
	// Getter
	PxRigidActor* getRigidBody();
	PxPhysics* getPhysics();
	PxMaterial* getMaterial();
	PxGeometry* getGeometry();
	bool getIsVehicule();
	// Setter
	void setRigidBody(PxRigidActor& _rigidActor);
	void setPhysics(PxPhysics& _gPhysics);
	void setMaterial(PxMaterial& _gMaterial);
	void setGeometry(PxGeometry& _geometry);

	ObjectPhysicalInstance();
	ObjectPhysicalInstance(PhysicalInitData& initData);
	virtual ~ObjectPhysicalInstance() = default;
	// RigidActor
	void setGlobalPose(const PxTransform& pose, bool autowake = true);
	// RigidDynamic
	void setMass(PxReal mass);
	void setMassSpaceInertiaTensor(const PxVec3& m);
	void setLinearDamping(PxReal linDamp);
	void setAngularDamping(PxReal angDamp);
	void setLinearVelocity(const PxVec3& linVel, bool autowake = true);
	void setAngularVelocity(const PxVec3& angVel, bool autowake = true);
	void addForce(const PxVec3& force, PxForceMode::Enum mode = PxForceMode::eFORCE, bool autowake = true);
	void addTorque(const PxVec3& torque, PxForceMode::Enum mode = PxForceMode::eFORCE, bool autowake = true);
	void clearForce(PxForceMode::Enum mode = PxForceMode::eFORCE);
	void clearTorque(PxForceMode::Enum mode = PxForceMode::eFORCE);

	// handle external physics process
	virtual void stepPhysics(){};
	virtual void onTrigger(PxActor*){};
	virtual void onContact(PxActor*){};
	//virtual void stepPhysics();
	void assignToObject(void* object);
	// RigidActor
	PxTransform getGlobalPose();
	
	// RigidDynamic
	PxReal getMass();
	PxVec3 getMassSpaceInertiaTensor();
	PxReal getLinearDamping();
	PxReal getAngularDamping();
	PxVec3 getLinearVelocity();
	PxVec3 getAngularVelocity();

};

