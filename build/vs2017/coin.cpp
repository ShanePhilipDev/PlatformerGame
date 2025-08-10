#include "coin.h"

// Constructor
Coin::Coin()
{
	// Coin will not be collected by default.
	collected_ = false;

	// Setting the game object type to be coin
	set_type(OBJECT_TYPE::COIN);
}


