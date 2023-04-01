#include "CollisionHandler.h"
#include <ObjectPhysicalInstance.h>

PxFilterFlags SimpleFilterShader(
	PxFilterObjectAttributes attributes0, PxFilterData ,
	PxFilterObjectAttributes attributes1, PxFilterData ,
	PxPairFlags& pairFlags, const void* , PxU32 )
{
	// let triggers through
	if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}
	// generate contacts for all that were not filtered above
	pairFlags = PxPairFlag::eCONTACT_DEFAULT;
	// trigger the contact callback for pairs (A,B) where
	// the filtermask of A contains the ID of B and vice versa.
	pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;

	return PxFilterFlag::eDEFAULT;
}

void setupFiltering(PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask)
{
	PxFilterData filterData;
	filterData.word0 = filterGroup; // word0 = own ID
	filterData.word1 = filterMask;  // word1 = ID mask to filter pairs that trigger a
	// contact callback;
	const PxU32 numShapes = actor->getNbShapes();
	PxShape* shapes[1];
	actor->getShapes(shapes, numShapes);
	PxShape* shape = shapes[0];
	shape->setSimulationFilterData(filterData);
}


void CollisionHandler::onTrigger(PxTriggerPair* pairs, PxU32 count) 
{
for (PxU32 i = 0; i < count; i++)
	{
		// ignore pairs when shapes have been deleted
		if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER |
			PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
			continue;

		if (!pairs[i].otherActor)
			continue;
		
		static_cast<ObjectPhysicalInstance*>(pairs[i].triggerActor->userData)->onTrigger(pairs[i].otherActor);
	}
}

void CollisionHandler::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* , PxU32 nbPairs)
{
	for (PxU32 i = 0; i < nbPairs; i++)
	{
		// ignore response on static objects
		if (pairHeader.actors[i]->is<PxRigidStatic>())
			continue;
		
		static_cast<ObjectPhysicalInstance*>(pairHeader.actors[i]->userData)->onContact(pairHeader.actors[1-i]);
	}
}