#include "ObjectPhysicalInstance.h"
#include "ObjectPhysicalInitData.h"
#include "PxPhysicsAPI.h"
using namespace physx;

#ifndef triggerzonetp_h
#define triggerzonetp_h
class TriggerZoneTp : public ObjectPhysicalInstance
{
public:
	TriggerZoneTp(PhysicalInitData& physicalInitData) : ObjectPhysicalInstance(physicalInitData) {}

	void onTrigger(PxActor*) override;
};

#endif