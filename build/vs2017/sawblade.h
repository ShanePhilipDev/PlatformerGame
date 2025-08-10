#pragma once
#include "game_object.h"
class Sawblade :
	public GameObject
{
public:
	Sawblade();

	// Functions for updating and initialising the sawblade.
	void Update(float frame_time);
	void Init(float vertical_speed, float horizontal_speed, float distance);
private:
	// Store the start position of the sawblade.
	b2Vec2 start_position_;

	// Different speed variables for the sawblade.
	float vertical_speed_;
	float base_vertical_speed_;
	float horizontal_speed_;
	float base_horizontal_speed_;
	float rotation_speed_;

	// The distance that the sawblade will move.
	float movement_distance_;
};

