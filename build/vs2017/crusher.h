#pragma once
#include "game_object.h"
class Crusher :
	public GameObject
{
public:
	Crusher();

	// Functions to update and initialise the crusher.
	void Update(float frame_time);
	void Init(float delay, float interval);

	// Returns whether the crusher is currently crushing or not.
	bool GetCrushing()
	{
		return crushing_;
	};

	// Set when the crate has finished crushing.
	void SetFinished(bool finished)
	{
		finished_ = finished;
	};
private:
	// Functions for crushing and reseting.
	void Crush();
	void Reset();

	// Bools for holding the state of the crusher.
	bool crushing_;
	bool finished_;

	// Variables for handling the time related properties of the crusher.
	float timer_;
	float interval_time_;
	float warning_time_;
	float reset_time_;

	// Save the start position so that the crusher can return to its original position once finished.
	b2Vec2 start_position_;
	
};

