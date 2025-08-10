#pragma once
#include "game_object.h"
class Coin :
	public GameObject
{
public:
	Coin();

	// Getter and setter function for the collected_ variable.
	void SetCollected(bool col)
	{
		collected_ = col;
	};
	bool GetCollected()
	{
		return collected_;
	};

private:
	// Bool to store whether the coin has been collected or not.
	bool collected_;
	
};

