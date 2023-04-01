#include "ObjectPhysicalInstance.h"
#include "ObjectPhysicalInitData.h"
#include "PxPhysicsAPI.h"
using namespace physx;

#ifndef triggerzonecave_h
#define triggerzonecave_h
class TriggerZoneCave : public ObjectPhysicalInstance
{
public:
	TriggerZoneCave(PhysicalInitData& physicalInitData) : ObjectPhysicalInstance(physicalInitData) {}

	void onTrigger(PxActor*) override;
};

#endif