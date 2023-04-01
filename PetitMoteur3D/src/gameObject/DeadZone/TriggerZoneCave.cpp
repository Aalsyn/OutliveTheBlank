#include "TriggerZoneCave.h"
#include "stdafx.h"
#include "MoteurWindows.h"

void TriggerZoneCave::onTrigger(PxActor*)
{
	const double tempsEntreLumiere = CMoteurWindows::GetInstance().getTempsEntreLumiere();
	if (tempsEntreLumiere <= 0)
	{
		GameStateManager& gsManager = GameStateManager::GetInstance();
		const bool isInCave = gsManager.getIsInCave();
		std::vector<Light>& lights = CMoteurWindows::GetInstance().getLights();
		for (Light& light : lights) {
			if (light.isSun)
			{
				if (!isInCave)
				{
					light.ambiant = XMVectorSet(.30f, .30f, .35f, 0.6f);// XMVectorSet(.20f, .20f, .25f, 0.5f);
					light.diffuse = XMVectorSet(0.0f, 0, 0, 0); //XMVectorSet(0.4f, 0.35f, 0.35f, 0.5f);
					light.specular = XMVectorSet(0.0f, 0, 0, 0); //XMVectorSet(0.25f, 0.25f, 0.25f, 0.5f);
				}
				else
				{
					light.ambiant = XMVectorSet(.40f, .40f, .50f, 1.0f); //XMVectorSet(.40f, .40f, .50f, 1.0f);
					light.diffuse = XMVectorSet(0.8f, 0.75f, 0.70f, 1.0f); //XMVectorSet(0.8f, 0.75f, 0.70f, 1.0f);
					light.specular = XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f); //XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f);
				}	
				break;
			}
		}
		if (isInCave) {
			gsManager.addCall(Call::ExitCave);
		}
		else {
			gsManager.addCall(Call::EnterCave);
		}
		CMoteurWindows::GetInstance().setTempsEntreLumiere(2);
	}
	
}
