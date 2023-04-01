#pragma once

#include "PxPhysicsAPI.h"
using namespace physx;


enum filterType {
	FILTER_BALL = 1,
	FILTER_CARGO = 2,
	FILTER_OBJECTIF = 3,
};


PxFilterFlags SimpleFilterShader(
	PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize);


void setupFiltering(PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask);

class CollisionHandler : public PxContactModifyCallback, public PxSimulationEventCallback {
public:
	// Implements PxSimulationEventCallback
	virtual void							onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs);
	virtual void							onTrigger(PxTriggerPair* pairs, PxU32 count) ;
	virtual void							onConstraintBreak(PxConstraintInfo*, PxU32) {}
	virtual void							onWake(PxActor**, PxU32) {}
	virtual void							onSleep(PxActor**, PxU32) {}
	virtual void							onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32) {}
	virtual void							onContactModify(PxContactModifyPair* const , PxU32 ) {};


};