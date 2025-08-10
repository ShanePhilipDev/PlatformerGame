#include "sawblade.h"

// Constructor
Sawblade::Sawblade()
{
	// Default values
	base_vertical_speed_ = 2.0f;
	base_horizontal_speed_ = 2.0f;
	movement_distance_ = 3.0f;
	rotation_speed_ = 1000.0f;
}

void Sawblade::Update(float frame_time)
{
	// If position exceeds the movement distance...
	if (GetBody()->GetPosition().x < start_position_.x - movement_distance_)
	{
		horizontal_speed_ = base_horizontal_speed_; // set the horizontal speed to the normal speed.
	}
	else if (GetBody()->GetPosition().x > start_position_.x + movement_distance_) // if exceeds distance in the other direction...
	{
		horizontal_speed_ = -base_horizontal_speed_; // set the horizontal speed to be negative.
	}
	
	if (GetBody()->GetPosition().y < start_position_.y) // If below the bottom of the sawblades path...
	{
		vertical_speed_ = base_vertical_speed_; // set the vertical speed to the normal speed.
	}
	else if (GetBody()->GetPosition().y > start_position_.y + movement_distance_)
	{
		vertical_speed_ = -base_vertical_speed_; // set the vertical speed to be negative.
	}

	GetBody()->SetTransform(b2Vec2(GetBody()->GetPosition().x + horizontal_speed_ * frame_time, GetBody()->GetPosition().y + vertical_speed_ * frame_time), GetBody()->GetAngle() - rotation_speed_ * frame_time);
}

void Sawblade::Init(float vertical_speed, float horizontal_speed, float distance)
{
	// Assign all of the variables values.
	start_position_ = GetBody()->GetPosition();
	base_vertical_speed_ = vertical_speed;
	base_horizontal_speed_ = horizontal_speed;
	vertical_speed_ = vertical_speed;
	horizontal_speed_ = horizontal_speed;
	movement_distance_ = distance;
}

