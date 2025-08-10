#pragma once
#include "game_object.h"


class Checkpoint :
	public GameObject
{
public:
	Checkpoint();

	// Setter and getter for triggered_ variable.
	void SetTriggered(bool triggered)
	{
		triggered_ = triggered;
	}
	bool GetTriggered()
	{
		return triggered_;
	}

	// Returns the position of the checkpoint.
	b2Vec2 GetPosition()
	{
		return GetBody()->GetPosition();
	}
	

private:
	// Bool to store if checkpoint has been activated
	bool triggered_;
};

