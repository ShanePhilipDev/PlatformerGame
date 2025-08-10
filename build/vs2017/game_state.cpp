#include "game_state.h"

GameState::GameState()
{
	current_state_ = State::SPLASH; // default to the splash state
}

State GameState::GetGameState()
{
	return current_state_;
}

void GameState::SetGameState(State s)
{
	current_state_ = s;
}
