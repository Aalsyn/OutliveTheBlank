#include <stdafx.h>
#include "TriggerZoneTp.h"
#include "MoteurWindows.h"

void TriggerZoneTp::onTrigger(PxActor*)
{
	const double tempsEntreTP = CMoteurWindows::GetInstance().getTempsEntreTP();
	if (tempsEntreTP <= 0)
	{
		GameStateManager& gsManager = GameStateManager::GetInstance();
		const bool isInMoon = gsManager.getIsInMoon();
		if (isInMoon)
		{
			gsManager.addCall(Call::TeleportToEarth);
		}
		else
		{
			gsManager.addCall(Call::TeleportToMoon);
		}
	}
	CMoteurWindows::GetInstance().setTempsEntreTP(2);
}
