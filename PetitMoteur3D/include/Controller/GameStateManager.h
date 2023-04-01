#include "Singleton.h"
#include <vector>

#ifndef gameStateManager_h
#define gameStateManager_h

enum class Call {
	EnterCave,
	ExitCave,
	EnterOption,
	ExitOption,
	Pause,
	Unpause,
	StartGame,
	Die,
	Reset,
	TeleportToMoon,
	TeleportToEarth,
	Quit,
	Nothing
};

class GameStateManager : public PM3D::CSingleton<GameStateManager> {
private:
	std::vector<Call> callStack;
	bool isInCave;
	bool isInMoon;
	bool inGame;
	bool inMainMenu;
	bool inOptions;
	int turnCount = 0;

	void ResetStates();
	void executeCall(const Call& call);

public:
	GameStateManager();
	~GameStateManager() = default;
	void addCall(const Call& call);
	void executeCalls();

	const bool getIsInCave() noexcept;
	const bool getIsInMoon() noexcept;
	const bool getInMainMenu() noexcept;
	const bool getInGame() noexcept;
	const bool getInOptions() noexcept;
	const int getTurnCount() noexcept;
};

#endif