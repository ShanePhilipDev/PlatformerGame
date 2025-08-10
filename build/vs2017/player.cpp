#include "player.h"

// Constructor
Player::Player()
{
	// Set default values.
	set_type(PLAYER);
	x_offset_ = 0.0f;
	y_offset_ = -0.8f;
	facing_left_ = false;
	falling_ = false;
	player_state_ = PlayerState::IDLE;
	old_player_state_ = PlayerState::RUNNING;
	timer_ = 0.0f;
	lives_ = 3.0f;
	respawn_position_ = b2Vec2(0.0f, 0.0f);
	death_reset_time_ = 2.0f;
	speed_ = 5.0f;
}

void Player::Update(float frame_time)
{
	// If the player is dead...
	if (player_state_ == PlayerState::DEAD)
	{
		// Wait until the timer exceeds the reset time.
		timer_ += frame_time;
		if (timer_ > death_reset_time_)
		{
			// Make player able to collide again, set velocity to 0, move to respawn position, set state to idle, decrease lives by 1 and reset timer.
			GetBody()->GetFixtureList()->SetSensor(false); 
			GetBody()->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
			GetBody()->SetTransform(respawn_position_, 0.0f);
			player_state_ = PlayerState::IDLE;
			lives_ -= 1;
			timer_ = 0.0f;
		}
	}

	// If the player's y velocity is below 0, falling will be true. Otherwise, falling will be false.
	if (GetBody()->GetLinearVelocity().y < 0.0f)
	{
		falling_ = true;
	}
	else
	{
		falling_ = false;
	}

	// If the player is falling and not kicking or dead, set player state to be falling.
	if (falling_ == true && player_state_ != PlayerState::KICKING && player_state_ != PlayerState::DEAD)
	{
		player_state_ = PlayerState::FALLING;
	}

	// If the player's state has changed...
	if (old_player_state_ != player_state_)
	{
		switch (player_state_) // Set player's animation based on their state.
		{
		case PlayerState::IDLE:
			if (idle_anim_)
			{
				anim_player_.set_clip(idle_anim_);
				anim_player_.set_looping(true);
				anim_player_.set_anim_time(0.0f);
				anim_player_.set_playback_speed(1.0f);
			}
			break;
		case PlayerState::RUNNING:
			if (run_anim_)
			{
				anim_player_.set_clip(run_anim_);
				anim_player_.set_looping(true);
				anim_player_.set_anim_time(0.0f);
				anim_player_.set_playback_speed(1.0f);
			}
			break;
		case PlayerState::KICKING:
			if (kick_anim_)
			{
				anim_player_.set_clip(kick_anim_);
				anim_player_.set_looping(false);
				anim_player_.set_anim_time(0.0f);
				anim_player_.set_playback_speed(3.0f);
			}
			break;
		case PlayerState::JUMPING:
			if (jump_anim_)
			{
				anim_player_.set_clip(jump_anim_);
				anim_player_.set_looping(false);
				anim_player_.set_anim_time(0.0f);
				anim_player_.set_playback_speed(1.0f);
			}
			break;
		case PlayerState::FALLING:
			if (fall_anim_)
			{
				anim_player_.set_clip(fall_anim_);
				anim_player_.set_looping(true);
				anim_player_.set_anim_time(0.0f);
				anim_player_.set_playback_speed(1.0f);
			}
			break;
		case PlayerState::LANDING:
			if (land_anim_)
			{
				anim_player_.set_clip(land_anim_);
				anim_player_.set_looping(false);
				anim_player_.set_anim_time(0.0f);
				anim_player_.set_playback_speed(3.0f);
			}
			break;
		case PlayerState::DANCING:
			if (dance_anim_)
			{
				anim_player_.set_clip(dance_anim_);
				anim_player_.set_looping(true);
				anim_player_.set_anim_time(0.0f);
				anim_player_.set_playback_speed(1.0f);
			}
			break;
		case PlayerState::DEAD:
			if (death_anim_)
			{
				anim_player_.set_clip(death_anim_);
				anim_player_.set_looping(true);
				anim_player_.set_anim_time(0.0f);
				anim_player_.set_playback_speed(1.0f);
			}
			break;
		default:
			break;
		}

		// Set old state to equal the new state.
		old_player_state_ = player_state_;
	}

	if (animated_mesh_)
	{
		// update the pose in the anim player from the animation
		if (anim_player_.Update(frame_time, animated_mesh_->bind_pose()))
		{
			// When the animation has finished, change the state based on which animation has just finished.
			if (anim_player_.clip() == kick_anim_)
			{
				player_state_ = PlayerState::LANDING;
			}
			else if (anim_player_.clip() == jump_anim_)
			{
				player_state_ = PlayerState::FALLING;
			}
			else if (anim_player_.clip() == land_anim_)
			{
				player_state_ = PlayerState::IDLE;
			}
		}

		// update the bone matrices that are used for rendering the character
		// from the newly updated pose in the anim player
		animated_mesh_->UpdateBoneMatrices(anim_player_.pose());

		// Calculating player's transformation matrix.
		gef::Vector4 rotation;
		gef::Vector4 position;

		// If the player is dead...
		if (player_state_ == PlayerState::DEAD)
		{
			position = gef::Vector4(body_->GetPosition().x + x_offset_, body_->GetPosition().y + y_offset_, 2.0f); // Apply offset and move player closer to camera so that he doesn't clip through the floor while falling when dead.
		}
		else
		{
			position = gef::Vector4(body_->GetPosition().x + x_offset_, body_->GetPosition().y + y_offset_, 0.0f); // Otherwise just apply the offset.
		}

		// If the player's facing left and not kicking, rotate to face left.
		if (facing_left_ && player_state_ != PlayerState::KICKING)
		{
			rotation = gef::Vector4(0.0f, gef::DegToRad(-90.0f), body_->GetAngle());
		}
		else if(player_state_ != PlayerState::KICKING) // If not kicking but not facing left, rotate to face right.
		{
			rotation = gef::Vector4(0.0f, gef::DegToRad(90.0f), body_->GetAngle());
		}
		else // Otherwise face forward while kicking.
		{
			rotation = gef::Vector4(0.0f, 0.0f, body_->GetAngle());
		}
		

		gef::Matrix44 rotX, rotY, rotZ, trans, final, scale; 

		// Scale the model down as it is much bigger than the other objects.
		scale.Scale(gef::Vector4(0.01, 0.01, 0.01));
		rotX.RotationX(rotation.x());
		rotY.RotationY(rotation.y());
		rotZ.RotationZ(rotation.z());
		trans.SetIdentity();
		trans.SetTranslation(position);

		// Calculate final transformation matrix.
		final = scale * rotX * rotY * rotZ * trans;

		// Apply final transformation matrix.
		animated_mesh_->set_transform(final);
	}
}

void Player::Init(gef::Platform* p)
{
	// Set pointer to platform.
	platform_ = p;

	// Load the scene that contains the player's model.
	player_scene_ = new gef::Scene();
	player_scene_->ReadSceneFromFile(*platform_, "player/player.scn");
	player_scene_->CreateMaterials(*platform_);

	// Get the player's mesh from the scene.
	player_mesh_ = MotionClipPlayer::GetFirstMesh(player_scene_, platform_);
	gef::Skeleton* skeleton = MotionClipPlayer::GetFirstSkeleton(player_scene_);

	// Setup the animated mesh.
	if (skeleton)
	{
		animated_mesh_ = new gef::SkinnedMeshInstance(*skeleton);
		anim_player_.Init(animated_mesh_->bind_pose());
		animated_mesh_->set_mesh(player_mesh_);
	}

	// Load all of the animations.
	kick_anim_ = MotionClipPlayer::LoadAnimation("player/anim-kick.scn", "", platform_);
	idle_anim_ = MotionClipPlayer::LoadAnimation("player/anim-idle.scn", "", platform_);
	jump_anim_ = MotionClipPlayer::LoadAnimation("player/anim-jump.scn", "", platform_);
	fall_anim_ = MotionClipPlayer::LoadAnimation("player/anim-fall.scn", "", platform_);
	land_anim_ = MotionClipPlayer::LoadAnimation("player/anim-land.scn", "", platform_);
	run_anim_ = MotionClipPlayer::LoadAnimation("player/anim-run.scn", "", platform_);
	death_anim_ = MotionClipPlayer::LoadAnimation("player/anim-death.scn", "", platform_);
	dance_anim_ = MotionClipPlayer::LoadAnimation("player/anim-dance.scn", "", platform_);

	// Set initial transform of the animated mesh.
	animated_mesh_->set_transform(this->transform());
}

void Player::Render(gef::Renderer3D* renderer_3d)
{
	// Render the animated mesh.
	if (animated_mesh_)
	{
		renderer_3d->DrawSkinnedMesh(*animated_mesh_, animated_mesh_->bone_matrices(), true);
	}
}

void Player::Jump()
{
	// Apply an upward force to the player, and set the state to jumping.
	const b2Vec2 jump = b2Vec2(0, 600);
	GetBody()->ApplyForceToCenter(jump, true);
	player_state_ = PlayerState::JUMPING;
}

void Player::MoveLeft(float frame_time)
{	
	// Move the player left and set the player's direction to left.
	b2Vec2 old_position = GetBody()->GetPosition();
	b2Vec2 new_position = b2Vec2(old_position.x - speed_ * frame_time, old_position.y);
	GetBody()->SetTransform(new_position, 0.0f);
	facing_left_ = true;

	// Change the state to be running if they are currently idle.
	if (player_state_ == PlayerState::IDLE)
	{
		player_state_ = PlayerState::RUNNING;
	}
}

void Player::MoveRight(float frame_time)
{
	// Move the player right and set the player's direction to right.
	b2Vec2 old_position = GetBody()->GetPosition();
	b2Vec2 new_position = b2Vec2(old_position.x + speed_ * frame_time, old_position.y);
	GetBody()->SetTransform(new_position, 0.0f);
	facing_left_ = false;

	// Change the state to be running if they are currently idle.
	if (player_state_ == PlayerState::IDLE)
	{
		player_state_ = PlayerState::RUNNING;
	}
}

void Player::Attack()
{
	// Set the player's state to be kicking.
	player_state_ = PlayerState::KICKING;
}

void Player::SetDead()
{
	// Apply an upward force to the player, set them to be a sensor so they fall through the ground, then set state to dead.
	b2Vec2 force = b2Vec2(0, 400);
	GetBody()->ApplyForceToCenter(force, true);
	GetBody()->GetFixtureList()->SetSensor(true);
	SetState(PlayerState::DEAD);
}



