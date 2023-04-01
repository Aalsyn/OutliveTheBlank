#include <string>
#include <codecvt>
#include <locale>
#include <chrono>
#include <vector>

#include "dispositif.h"
#include "GestionnaireDeTextures.h"
#include "Singleton.h"
#include "Vehicule.h"

#include "AfficheurSprite.h"
#include "AfficheurTexte.h"
#ifndef uiController_h
#define uiController_h

using namespace std;
namespace PM3D{
	class UIController : public CSingleton <UIController> {

	public:
		void RenderMenu(unique_ptr<CAfficheurTexte>& pTexteReprendre, unique_ptr<CAfficheurTexte>& pTexteReset, 
			unique_ptr<CAfficheurTexte>& pTexteOptions, unique_ptr<CAfficheurTexte>& pTexteQuitter, int choice);
		void RenderHUD(unique_ptr<CAfficheurTexte> &pTexteChrono, unique_ptr<CAfficheurTexte> &pTexteSpeed,wstring chronoToPrint, wstring speedToPrint);

	};
}
#endif