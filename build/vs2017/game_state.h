#pragma once

// Enum with all of the possible states.
enum class State {
	SPLASH,
	MENU,
	PAUSED,
	LEVEL,
	WIN,
	LOSE
};

class GameState
{
public:
	GameState();
	
	// Getter and setter for the game's state
	State GetGameState();
	void SetGameState(State s);
private:
	// Holds the current state
	State current_state_;
};

