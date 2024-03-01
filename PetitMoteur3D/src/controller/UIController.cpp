#include <stdafx.h>
#include "UIController.h"
#include "Converter.h"
#include "MoteurWindows.h"
#include "cameraController.h"

using namespace DirectX;
using namespace PM3D;
using namespace MyConverter;


void UIController::RenderHUD(unique_ptr<CAfficheurTexte> &pTexteChrono, unique_ptr<CAfficheurTexte> &pTexteSpeed, wstring chronoToPrint, wstring speedToPrint)
{
	pTexteChrono->Ecrire(chronoToPrint);
	pTexteSpeed->Ecrire(speedToPrint);
}

void UIController::RenderMenu(unique_ptr<CAfficheurTexte> &pTexteReprendre, unique_ptr<CAfficheurTexte> &pTexteReset,
	unique_ptr<CAfficheurTexte> &pTexteOptions, unique_ptr<CAfficheurTexte> &pTexteQuitter, int choice) {
	if (choice == 0) pTexteReprendre->Ecrire(L">>> Reprendre");
	else pTexteReprendre->Ecrire(L"Reprendre");
	if (choice == 1) pTexteReset->Ecrire(L">>> Restart");
	else pTexteReset->Ecrire(L"Restart");
	if (choice == 2) pTexteOptions->Ecrire(L">>> Options");
	else pTexteOptions->Ecrire(L"Options");
	if (choice == 3) pTexteQuitter->Ecrire(L">>> Quitter");
	else pTexteQuitter->Ecrire(L"Quitter");
}

