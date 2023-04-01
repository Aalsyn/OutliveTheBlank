#include "stdafx.h"
#include "GameStateManager.h"
#include "MoteurWindows.h"

GameStateManager::GameStateManager() {
	ResetStates();
}

void GameStateManager::ResetStates() {
	 isInCave = false;
	 isInMoon = false;
	 inGame = false;
	 inMainMenu = true;
	 inOptions = false;
	 turnCount = 0;
}

void GameStateManager::addCall(const Call& call) {
	callStack.push_back(call);
}

void GameStateManager::executeCall(const Call& call) {
	switch (call)
	{
	case Call::EnterCave: 
		isInCave = true;
		break;
	case Call::ExitCave:
		isInCave = false;
		break;
	case Call::EnterOption:
		CMoteurWindows::GetInstance().resetOption();
		inOptions = true;
		break;
	case Call::ExitOption:
		inOptions = false;
		break;
	case Call::Pause:
		TimeController::GetInstance().pause();
		inGame = false;
		break;
	case Call::Unpause:
		TimeController::GetInstance().resume();
		inGame = true;
		inOptions = false;
		break;
	case Call::StartGame:
		inMainMenu = false;
		inGame = true;
		isInMoon = false;
		turnCount = 0;
		SoundController::GetInstance().stopMenuSoundtrack();
		SoundController::GetInstance().playRaceSoundtrack();
		CMoteurWindows::GetInstance().ResetGame();
		break;
	case Call::Reset:
		ResetStates();
		inGame = true;
		inMainMenu = false;
		
		CMoteurWindows::GetInstance().ResetGame();
		break;
	case Call::TeleportToEarth:
		CMoteurWindows::GetInstance().Teleport(isInMoon);
		turnCount++;
		isInMoon = false;
		break;
	case Call::TeleportToMoon:
		CMoteurWindows::GetInstance().Teleport(isInMoon);
		isInMoon = true;
		break;
	case Call::Quit:
		CMoteurWindows::GetInstance().cameraController.setActiveCamera(2);
		SoundController::GetInstance().stopRaceSoundtrack();
		SoundController::GetInstance().playMenuSoundtrack();
		inGame = false;
		inMainMenu = true;
		break;
	default:
		break;
	}
}


void GameStateManager::executeCalls() {
	for (const Call& call : callStack) {
		executeCall(call);
	}
	callStack.clear();
}

const bool GameStateManager::getIsInMoon() noexcept {
	return isInMoon;
}

const bool GameStateManager::getIsInCave() noexcept {
	return isInCave;
}

const bool GameStateManager::getInMainMenu() noexcept {
	return inMainMenu;
}

const bool GameStateManager::getInGame() noexcept {
	return inGame;
}

const bool GameStateManager::getInOptions() noexcept {
	return inOptions;
}

const int GameStateManager::getTurnCount() noexcept {
	return turnCount;
}