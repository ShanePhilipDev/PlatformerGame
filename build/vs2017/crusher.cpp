#include "crusher.h"

// Constructor.
Crusher::Crusher()
{
	// Set default values.
	timer_ = 0.0f;
	interval_time_ = 3.0f; // 3 seconds between crushing (not including reset time).
	reset_time_ = 2.0f; // The crusher will remain down for 2 seconds before reseting.
	crushing_ = false;
	finished_ = false;
	warning_time_ = 0.5f; // 0.5 second warning that it's about to crush
}

void Crusher::Update(float frame_time)
{
	// Increment the timer by the frame time.
	timer_ += frame_time;

	// If the crusher is not crushing or finished and it is within the warning time period...
	if (timer_ > interval_time_ - warning_time_ && !crushing_ && !finished_) 
	{
		// To warn the player that the crusher is about to drop, it will slowly move downwards as a precursor.
		GetBody()->SetTransform(b2Vec2(GetBody()->GetPosition().x, GetBody()->GetPosition().y - frame_time), 0);
	}

	// Crush once the required time has passed and its not already crushing or finished.
	if (timer_ > interval_time_ && !crushing_ && !finished_)
	{
		Crush();
	}

	// Once finished, it will return to being a static body.
	if (finished_)
	{
		GetBody()->SetType(b2_staticBody);
		crushing_ = false;
	}

	// If the crusher has finished crushing and the timer exceeds the reset time...
	if (finished_ && timer_ > reset_time_)
	{
		// Move the crusher back up until it reaches its start position, then reset the crusher.
		if (GetBody()->GetPosition().y < start_position_.y)
		{
			GetBody()->SetTransform(b2Vec2(GetBody()->GetPosition().x, GetBody()->GetPosition().y + 2 * frame_time), 0);
		}
		else
		{
			Reset();
		}
	}
}

void Crusher::Init(float delay, float interval)
{
	// Set values for delay, interval and start position.
	timer_ = -delay; // timer stars at a negative value, so it will take slightly longer to reach the crush time for the first run making it out of sync with other crushers unless they have the same delay.
	interval_time_ = interval;
	start_position_ = GetBody()->GetPosition();
}

// Crushing function. When this is called the crusher will be pushed down into the ground with a great force.
void Crusher::Crush()
{
	// Set the crushing status to be true.
	crushing_ = true;

	// Reset the timer.
	timer_ = 0.0f;

	// Set the body type to dynamic so it can move.
	GetBody()->SetType(b2_dynamicBody);

	// Apply the downward force.
	b2Vec2 force = b2Vec2(0, -8000);
	GetBody()->ApplyForceToCenter(force, false);
}

void Crusher::Reset()
{
	// Reset crushing, finished and timer variables.
	crushing_ = false;
	finished_ = false;
	timer_ = 0.0f;
}
