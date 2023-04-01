#include "DeadZone.h"
#include "stdafx.h"
#include "MoteurWindows.h"


void DeadZone::onTrigger(PxActor*)
{
	GameStateManager::GetInstance().addCall(Call::Reset);
	SoundController::GetInstance().playFall();
}
