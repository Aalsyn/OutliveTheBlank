#include <stdafx.h>
#include "InputController.h"
#include "Converter.h"
#include "MoteurWindows.h"
#include "Vehicule.h"
#include "cameraController.h"
#include "TimeController.h"
#include "SoundController.h"
#include <WinUser.h>

using namespace DirectX;
using namespace PM3D;
using namespace MyConverter;

constexpr float turnAngle = XM_PI * 2.0f /180.0f;
float mouseSensitivity = 1.0f;

CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
CDIManipulateur& rGS = rMoteur.GetGestionnaireDeSaisie();
CameraController* camControl = &rMoteur.cameraController; 


void InputController::HandleKeyboardInput() {
	
	Vehicule* _vehicule = rMoteur.vehicule;
	GameStateManager& gsManager = GameStateManager::GetInstance();
	
	
	if (!gsManager.getInMainMenu()) {
		if (gsManager.getInGame()) {
			if (_vehicule->isOnGround) {
				//GROUND CONTROLS
				if (rGS.ToucheAppuyee(DIK_W))
				{
					SoundController::GetInstance().stopIdle();
					SoundController::GetInstance().playMoving();
					_vehicule->accelerate();
				}
				else {
					SoundController::GetInstance().stopMoving();
					SoundController::GetInstance().playIdle();
				}
				if (rGS.ToucheAppuyee(DIK_S))
				{
					_vehicule->decelerate();
				}
				if (rGS.ToucheAppuyee(DIK_A))
				{
					_vehicule->turnY(-1.0f);
				}
				if (rGS.ToucheAppuyee(DIK_D))
				{
					_vehicule->turnY(1.0f);
				}
				if (rGS.ToucheAppuyee(DIK_SPACE) && !holdingSpace)
				{
					holdingSpace = true;
					_vehicule->isOnGround = false;
					_vehicule->jump(16.0f);
				}
			}
			else {
				//AERIAL CONTROLS
				if (rGS.ToucheAppuyee(DIK_W))
				{
					SoundController::GetInstance().stopIdle();
					SoundController::GetInstance().playMoving();
					_vehicule->strafeForward();
				}
				else {
					SoundController::GetInstance().stopMoving();
					SoundController::GetInstance().playIdle();
				}
				if (rGS.ToucheAppuyee(DIK_S))
				{
					_vehicule->strafeBackward();
				}
				if (rGS.ToucheAppuyee(DIK_A))
				{
					_vehicule->strafeLeft();
				}
				if (rGS.ToucheAppuyee(DIK_D))
				{
					_vehicule->strafeRight();
				}
			}
			if (_vehicule->isOnGround && !_vehicule->isGrounded) {
				//Landing behavior
				_vehicule->isGrounded = true;
				_vehicule->projectSpeedOnDirection();
			}

		}
		else {
			if (rGS.ToucheAppuyee(DIK_DOWNARROW) && !holdingDown) {
				holdingDown = true;
				if (selection != MenuSelector::Quitter) {
					changeMenuSelection(true);
					rMoteur.selection++;
				}
			}
			if (rGS.ToucheAppuyee(DIK_UPARROW) && !holdingUp) {
				holdingUp = true;
				if (selection != MenuSelector::Reprendre) {
					changeMenuSelection(false);
					rMoteur.selection--;
				}
			}
			if (rGS.ToucheAppuyee(DIK_RETURN)) {
				MenuEnterKeyPressed();
			}
		}

		//UTILS CONTROLS
		if (rGS.ToucheAppuyee(DIK_TAB) && !holdingTab)
		{
			holdingTab = true;
			camControl->setActiveCamera(1 - camControl->getActiveCameraID());
		}
		//DEV TP
		if (rGS.ToucheAppuyee(DIK_T) && !holdingT) {
			holdingT = true;
			_vehicule->teleport(PxVec3(-3455, 30, -268), 2.0f);
		}
		//change ambiant lights
		if (rGS.ToucheAppuyee(DIK_L) && !holdingLight)
		{
			isShadow = !isShadow;
			holdingLight = true;
			XMVECTOR changingEffect = XMVectorSet(0.2f, 0.0f, 0.0f, 0.0f);
			if (isShadow)
			{
				auto& lights = CMoteurWindows::GetInstance().getLights();
				for (auto& light : lights) {
					light.ambiant = XMVectorMax(XMVectorSet(0, 0, 0, 0), XMVectorSubtract(light.ambiant, changingEffect));
					/*light.diffuse = XMVectorMax(XMVectorSet(0, 0, 0, 0), XMVectorSubtract(light.diffuse, changingEffect));
					light.specular = XMVectorMax(XMVectorSet(0, 0, 0, 0), XMVectorSubtract(light.specular, changingEffect));*/
				}
			}
			else
			{
				auto& lights = CMoteurWindows::GetInstance().getLights();
				for (auto& light : lights) {
					light.ambiant = XMVectorMin(XMVectorSet(1, 1, 1, 1), XMVectorAdd(light.ambiant, changingEffect));
					//light.diffuse = XMVectorMin(XMVectorSet(1, 1, 1, 1), XMVectorAdd(light.diffuse, changingEffect));
					//light.specular = XMVectorMin(XMVectorSet(1, 1, 1, 1), XMVectorAdd(light.specular, changingEffect));

				}
			}
		}
		//Menu open 
		if (rGS.ToucheAppuyee(DIK_ESCAPE) && !holdingEsc)
		{
			//switch game state to pause
			//start counting pause time
			holdingEsc = true;
			if (gsManager.getInGame()) {
				gsManager.addCall(Call::Pause);
			}
			else {
				gsManager.addCall(Call::Unpause);
			}
			SoundController::GetInstance().stopIdle();
			SoundController::GetInstance().stopMoving();
			SoundController::GetInstance().stopMonster();

		}

		//Sensibility managing
		if (!rGS.ToucheAppuyee(DIK_TAB)) {
			holdingTab = false;
		}
		if (!rGS.ToucheAppuyee(DIK_L)) {
			holdingLight = false;
		}
		if (!rGS.ToucheAppuyee(DIK_ESCAPE)) {
			holdingEsc = false;
		}
		if (!rGS.ToucheAppuyee(DIK_DOWNARROW)) {
			holdingDown = false;
		}
		if (!rGS.ToucheAppuyee(DIK_UPARROW)) {
			holdingUp = false;
		}
		if (!rGS.ToucheAppuyee(DIK_SPACE)) {
			holdingSpace = false;
		}
		if (!rGS.ToucheAppuyee(DIK_T)) {
			holdingT = false;
		}
		if (_vehicule->isOnGround && !_vehicule->IsSliding()) {
			_vehicule->projectSpeedOnDirection();
		}
	}
	
	
}


void InputController::HandleMouseInput() {
	const DIMOUSESTATE& delta = rGS.EtatSouris();
	Vehicule* _vehicule = rMoteur.vehicule;
	GameStateManager& gsManager = GameStateManager::GetInstance();
	if (gsManager.getInOptions()) {
		POINT cursor = rGS.GetMousePosition();
		if ((delta.rgbButtons[0] & 0x80) && !isShooting) {
			isShooting = true;
			if (cursor.x > 850 && cursor.y > 72 && cursor.x < 1106 && cursor.y < 200) {
				//audio || facile || audio 25 || 1280x1024 (5:4)
				OptionButtonClicked(1);
			}
			else if (cursor.x > 850 && cursor.y > 272 && cursor.x < 1106 && cursor.y < 400) {
				//video || moyen || audio 50 || 1600x1200 (4:3)
				OptionButtonClicked(2);
			}
			else if (cursor.x > 850 && cursor.y > 472 && cursor.x < 1106 && cursor.y < 600) {
				//difficulte || difficile || audio 75 || 1680x1050(16:10)
				OptionButtonClicked(3);
			}
			else if (cursor.x > 850 && cursor.y > 672 && cursor.x < 1106 && cursor.y < 800) {
				//touches || audio 100 || 1920x1080(16:9)
				OptionButtonClicked(4);
			}
			else if (cursor.x > 850 && cursor.y > 872 && cursor.x < 1106 && cursor.y < 1000) {
				//retour
				OptionButtonClicked(5);
			}
		}
	}
	
	else if (gsManager.getInMainMenu()) {
		POINT cursor = rGS.GetMousePosition();
		if ((delta.rgbButtons[0] & 0x80) && !isShooting) {
			isShooting = true;
			if (cursor.x > 850 && cursor.y > 322 && cursor.x < 1106 && cursor.y < 450) {
				//jouer
				gsManager.addCall(Call::StartGame);
			}
			else if (cursor.x > 850 && cursor.y > 472 && cursor.x < 1106 && cursor.y < 600) {
				//options
				gsManager.addCall(Call::EnterOption);
			}
			else if (cursor.x > 850 && cursor.y > 622 && cursor.x < 1106 && cursor.y < 750) {
				//Quitter
				PostQuitMessage(0);
			}
		}
	}
	else {
		if (gsManager.getInGame()) {
			if (_vehicule->isOnGround) {
				//GROUND CONTROLS
				_vehicule->turnY(0.8f * delta.lX * turnAngle * mouseSensitivity);
			}
			else {
				//AERIAL CONTROLS
				_vehicule->turnY(2.4f * delta.lX * turnAngle * mouseSensitivity);
			}

			if ((delta.rgbButtons[0] & 0x80) && !isShooting ) {
				_vehicule->tryShoot();
				isShooting = true;
			}

		}
		else {
			POINT cursor = rGS.GetMousePosition();
			if (delta.rgbButtons[0] & 0x80) {
				isShooting = true;
				if (cursor.x > 875 && cursor.y > 250 && cursor.x < 1095 && cursor.y < 300) {
					//reprendre clicked
					gsManager.addCall(Call::Unpause);
				}
				else if (cursor.x > 895 && cursor.y > 400 && cursor.x < 1095 && cursor.y < 450) {
					//restart clicked
					gsManager.addCall(Call::Reset);
				}
				else if (cursor.x > 895 && cursor.y > 550 && cursor.x < 1095 && cursor.y < 600) {
					//options clicked
					gsManager.addCall(Call::EnterOption);
				}
				else if (cursor.x > 895 && cursor.y > 700 && cursor.x < 1095 && cursor.y < 750) {
					//quitter clicked
					gsManager.addCall(Call::Quit);
				}

			}
#pragma region hover
			if (cursor.x > 855 && cursor.y > 250 && cursor.x < 1095 && cursor.y < 300) {
				//reprendre clicked
				rMoteur.selection = 0;
				selection = MenuSelector::Reprendre;
			}
			else if (cursor.x > 895 && cursor.y > 400 && cursor.x < 1095 && cursor.y < 450) {
				//restart clicked
				rMoteur.selection = 1;
				selection = MenuSelector::Reset;
			}
			else if (cursor.x > 895 && cursor.y > 550 && cursor.x < 1095 && cursor.y < 600) {
				//options clicked
				rMoteur.selection = 2;
				selection = MenuSelector::Options;
			}
			else if (cursor.x > 895 && cursor.y > 700 && cursor.x < 1095 && cursor.y < 750) {
				//quitter clicked
				rMoteur.selection = 3;
				selection = MenuSelector::Quitter;
			}
#pragma endregion
		}
	}

	if (!(delta.rgbButtons[0] & 0x80)) isShooting = false;
	
}

void InputController::OptionButtonClicked(int nbr) {
	GameStateManager& gsManager = GameStateManager::GetInstance();
	switch (nbr) {
		case 1:
			if (rMoteur.optionIsNone()) {
				rMoteur.setOptionAudio();
			}
			else if (rMoteur.optionIsAudio()) {
				//set audio to 25percent
				SoundController::GetInstance().changeVolume(0.25);
				rMoteur.resetOption();
			}
			else if (rMoteur.optionIsVideo()) {
				//set resolution to 1280*1024 5:4
				rMoteur.resetOption();
			}
			else if (rMoteur.optionIsDifficulte()) {
				//set difficulty to easy
				rMoteur.monstres->setVitesse(rMoteur.vehicule->getVitMax() * 0.6f);
				rMoteur.difficulte = 0.5;
				rMoteur.resetOption();
			}
			else if (rMoteur.optionIsTouche()) {
				mouseSensitivity = 0.25f;
				rMoteur.resetOption();
			}
			break;
		case 2:
			if (rMoteur.optionIsNone()) {
				rMoteur.setOptionVideo();
			}
			else if (rMoteur.optionIsAudio()) {
				//set audio to 50percent
				SoundController::GetInstance().changeVolume(0.5);
				rMoteur.resetOption();
			}
			else if (rMoteur.optionIsVideo()) {
				//set resolution to 1600x1200 (4:3)
				rMoteur.resetOption();
			}
			else if (rMoteur.optionIsDifficulte()) {
				//set difficulty to moyen
				rMoteur.monstres->setVitesse(rMoteur.vehicule->getVitMax() * 0.8f);
				rMoteur.difficulte = 0.75;
				rMoteur.resetOption();
			}
			else if (rMoteur.optionIsTouche()) {
				mouseSensitivity = 0.5f;
				rMoteur.resetOption();
			}
			break;
		case 3:
			if (rMoteur.optionIsNone()) {
				rMoteur.setOptionDifficulte();
				
			}
			else if (rMoteur.optionIsAudio()) {
				//set audio to 75percent
				SoundController::GetInstance().changeVolume(0.75);
				rMoteur.resetOption();
			}
			else if (rMoteur.optionIsVideo()) {
				//set resolution to 1680x1050(16:10)
				rMoteur.resetOption();
			}
			else if (rMoteur.optionIsDifficulte()) {
				//set difficulty to difficile
				rMoteur.monstres->setVitesse(rMoteur.vehicule->getVitMax());
				rMoteur.difficulte = 1.0;
				rMoteur.resetOption();
			}
			else if (rMoteur.optionIsTouche()) {
				mouseSensitivity = 0.75f;
				rMoteur.resetOption();
			}
			break;
		case 4:
			if (rMoteur.optionIsNone()) {
				rMoteur.setOptionTouche();
			}
			else if (rMoteur.optionIsAudio()) {
				//set audio to 100percent
				SoundController::GetInstance().changeVolume(1.0);
				rMoteur.resetOption();
			}
			else if (rMoteur.optionIsVideo()) {
				//set resolution to 1920x1080(16:9)
				rMoteur.resetOption();
			}
			else if (rMoteur.optionIsDifficulte()) {
				//set difficulty to fury
				rMoteur.monstres->setVitesse(rMoteur.vehicule->getVitMax() * 1.5f);
				rMoteur.difficulte = 1.30f;
				rMoteur.resetOption();
			}
			else if (rMoteur.optionIsTouche()) {
				mouseSensitivity = 1.0f;
				rMoteur.resetOption();
			}
			break;
		case 5:
			if (rMoteur.optionIsNone()) {
				gsManager.addCall(Call::ExitOption);
			}
			else {
				rMoteur.resetOption();
			}
			
			break;
		default:
			break;
	}
}

void InputController::changeMenuSelection(bool down) {
	switch (selection)
	{
	case MenuSelector::Reprendre:
		if (down) selection = MenuSelector::Reset;
		break;
	case MenuSelector::Reset:
		if (down) {
			selection = MenuSelector::Options;
		}

		else {
			selection = MenuSelector::Reprendre;
		}
		break;
	case MenuSelector::Options:
		if (down) {
			selection = MenuSelector::Quitter;
		}
		else {
			selection = MenuSelector::Reset;
		}

		break;
	case MenuSelector::Quitter:
		if (!down) selection = MenuSelector::Options;
		break;
	default:
		break;
	}
}

void InputController::MenuEnterKeyPressed() {
	GameStateManager& gsManager = GameStateManager::GetInstance();
	switch (selection)
	{
	case MenuSelector::Reprendre:
		gsManager.addCall(Call::Unpause);
		break;
	case MenuSelector::Reset:
		//launch the reset function
		gsManager.addCall(Call::Reset);
		break;
	case MenuSelector::Options:
		//open the options
		gsManager.addCall(Call::EnterOption);
		break;
	case MenuSelector::Quitter:
		//leave the game
		//notify uicontroller that it needs to charge options
		gsManager.addCall(Call::Quit);
		break;
	default:
		break;
	};
}