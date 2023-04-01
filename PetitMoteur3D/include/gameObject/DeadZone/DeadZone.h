#include "ObjectPhysicalInstance.h"
#include "ObjectPhysicalInitData.h"
#include "PxPhysicsAPI.h"

using namespace physx;

#ifndef deadzone_h
#define deadzone_h
class DeadZone : public ObjectPhysicalInstance
{
public:

	DeadZone(PhysicalInitData& physicalInitData) : ObjectPhysicalInstance(physicalInitData) {}
	~DeadZone() {
	
	}

	void onTrigger(PxActor*) override;
};

#endif