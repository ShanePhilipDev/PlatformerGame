#include "enemy.h"

// Constructor
Enemy::Enemy()
{
	// Setting default values.
	set_type(ENEMY);
	x_offset_ = 0.15f;
	y_offset_ = -0.8f;
	facing_left_ = true;
	enemy_state_ = EnemyState::RUNNING;
	old_enemy_state_ = EnemyState::IDLE;
	walk_distance_ = 0.0f;
	idle_time_ = 0.0f;
	start_position_ = b2Vec2(0.0f, 0.0f);
	timer_ = 0.0f;
	speed_ = 4.0f;
}

void Enemy::Update(float frame_time)
{
	// Increment timer by frame time.
	timer_ += frame_time;

	// If the enemy is idle...
	if (enemy_state_ == EnemyState::IDLE)
	{
		// If the timer exceeds the idle time, switch direction and set state as running.
		if (timer_ > idle_time_)
		{
			facing_left_ = !facing_left_;
			enemy_state_ = EnemyState::RUNNING;
		}
	}
	else if (facing_left_ && enemy_state_ == EnemyState::RUNNING) // If the enemy is facing left and running, move the enemy left until it exceeds the walk distance.
	{
		b2Vec2 old_position = GetBody()->GetPosition();
		const b2Vec2 new_position = b2Vec2(old_position.x - speed_ * frame_time, old_position.y);
		GetBody()->SetTransform(new_position, 0.0f);
		if (new_position.x < (start_position_.x - walk_distance_)) // If the enemy's position exceeds the walk distance, reset the timer and set state to idle.
		{
			timer_ = 0.0f;
			enemy_state_ = EnemyState::IDLE;
		}
	}
	else if (!facing_left_ && enemy_state_ == EnemyState::RUNNING) // If the enemy is facing right and running, move the enemy right until it exceeds the walk distance.
	{
		b2Vec2 old_position = GetBody()->GetPosition();
		const b2Vec2 new_position = b2Vec2(old_position.x + speed_ * frame_time, old_position.y);
		GetBody()->SetTransform(new_position, 0.0f);
		if (new_position.x > (start_position_.x + walk_distance_)) // If the enemy's position exceeds the walk distance, reset the timer and set state to idle.
		{
			timer_ = 0.0f;
			enemy_state_ = EnemyState::IDLE;
		}
	}

	// The x offset will change depending on the enemy's orientation.
	if (facing_left_)
	{
		x_offset_ = 0.15f;
	}
	else if (!facing_left_)
	{
		x_offset_ = -0.15f;
	}

	// If the enemy's state has changed...
	if (old_enemy_state_ != enemy_state_)
	{
		switch (enemy_state_) // Pick an animation based on the enemy's state.
		{
		case EnemyState::IDLE:
			if (idle_anim_)
			{
				anim_player_.set_clip(idle_anim_);
				anim_player_.set_looping(true);
				anim_player_.set_anim_time(0.0f);
				anim_player_.set_playback_speed(1.0f);
			}
			break;
		case EnemyState::RUNNING:
			if (run_anim_)
			{
				anim_player_.set_clip(run_anim_);
				anim_player_.set_looping(true);
				anim_player_.set_anim_time(0.0f);
				anim_player_.set_playback_speed(1.0f);
			}
			break;
		case EnemyState::DEAD:
			if (idle_anim_)
			{
				anim_player_.set_clip(idle_anim_);
				anim_player_.set_looping(true);
				anim_player_.set_anim_time(0.0f);
				anim_player_.set_playback_speed(1.0f);
			}
			break;
		default:
			break;
		}
		
		// Set old state to be equal to the new state.
		old_enemy_state_ = enemy_state_;
	}

	// Play the animation and update the mesh's transform.
	if (animated_mesh_)
	{
		// update the pose in the anim player from the animation
		anim_player_.Update(frame_time, animated_mesh_->bind_pose());

		// update the bone matrices that are used for rendering the character
		// from the newly updated pose in the anim player
		animated_mesh_->UpdateBoneMatrices(anim_player_.pose());

		// Apply offset to the body's position.
		gef::Vector4 position(body_->GetPosition().x + x_offset_, body_->GetPosition().y + y_offset_, 0.0f);
	
		gef::Vector4 rotation;

		// Flip model to be facing left or right.
		if (facing_left_)
		{
			rotation = gef::Vector4(0.0f, gef::DegToRad(-90.0f), body_->GetAngle());
		}
		else
		{
			rotation = gef::Vector4(0.0f, gef::DegToRad(90.0f), body_->GetAngle());
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

void Enemy::Init(gef::Platform* p, gef::Scene* enemy_scene)
{
	// Set platform pointer.
	platform_ = p;

	// Get the enemy's mesh and skeleton from the scene.
	gef::Mesh* enemy_mesh = MotionClipPlayer::GetFirstMesh(enemy_scene, platform_);
	gef::Skeleton* skeleton = MotionClipPlayer::GetFirstSkeleton(enemy_scene);

	// Create the animated mesh.
	if (skeleton)
	{
		animated_mesh_ = new gef::SkinnedMeshInstance(*skeleton);
		anim_player_.Init(animated_mesh_->bind_pose());
		animated_mesh_->set_mesh(enemy_mesh);
	}
	
	// Load the animations.
	idle_anim_ = MotionClipPlayer::LoadAnimation("enemy/anim-zombie-idle.scn", "", platform_);
	run_anim_ = MotionClipPlayer::LoadAnimation("enemy/anim-zombie-run.scn", "", platform_);

	// Set mesh's initial transform.
	animated_mesh_->set_transform(this->transform());

	// Save the body's start position.
	start_position_ = GetBody()->GetPosition();
}

void Enemy::Render(gef::Renderer3D* renderer_3d)
{
	// Render the animated mesh.
	if (animated_mesh_) 
	{
		renderer_3d->DrawSkinnedMesh(*animated_mesh_, animated_mesh_->bone_matrices(), true);
	}
}

void Enemy::Reset()
{
	// Reset the enemy's values back to default.
	GetBody()->SetAwake(true);
	GetBody()->GetFixtureList()->SetSensor(false);
	GetBody()->SetTransform(start_position_, 0);
	GetBody()->SetLinearVelocity(b2Vec2(0, 0));
	SetState(EnemyState::IDLE);
	timer_ = 0;
}

void Enemy::SetDead(Direction dir)
{
	// Set a force based on the direction that the enemy was killed from. 
	b2Vec2 force;
	if (dir == Direction::LEFT) // if from left
	{
		force = b2Vec2(5000, 2000);
	}
	else if (dir == Direction::RIGHT) // if from right
	{
		force = b2Vec2(-5000, 2000);
	}
	else // if from above
	{
		force = b2Vec2(0, -100);
	}
	
	// Apply the force, set the enemy to be a sensor so it passes through objects, and set the enemy state to dead.
	GetBody()->ApplyForceToCenter(force, true);
	GetBody()->GetFixtureList()->SetSensor(true);
	SetState(EnemyState::DEAD);
}

void Enemy::SetPath(float distance, float time)
{
	// Set the walk distance and idle time variables.
	walk_distance_ = distance;
	idle_time_ = time;
}

