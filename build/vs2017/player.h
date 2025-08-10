#pragma once
#include "game_object.h"
#include <graphics/skinned_mesh_instance.h>
#include <animation/animation.h>
#include <graphics/scene.h>
#include "motion_clip_player.h"
#include "graphics/renderer_3d.h"
#include "maths/math_utils.h"

// The possible states for the player.
enum class PlayerState {
	IDLE,
	RUNNING,
	KICKING,
	JUMPING,
	FALLING,
	LANDING,
	DANCING,
	DEAD
};

class Player : public GameObject
{
public:
	Player();
	
	// Functions for updating, initialising and rendering the player.
	void Update(float frame_time);
	void Init(gef::Platform* p);
	void Render(gef::Renderer3D* renderer_3d);

	// Functions for player movement and actions.
	void Jump();
	void MoveLeft(float frame_time);
	void MoveRight(float frame_time);
	void Attack();

	// Function to set the player as dead.
	void SetDead();

	// Function for altering the player's respawn position when new checkpoints are reached.
	void SetRespawnPosition(b2Vec2 position)
	{
		respawn_position_ = position;
	}

	// Getter and setter for the player's state.
	PlayerState GetState()
	{
		return player_state_;
	};
	void SetState(PlayerState state)
	{
		player_state_ = state;
	};

	// Getter and setter for the player's lives.
	int GetLives()
	{
		return lives_;
	};
	void SetLives(int lives)
	{
		lives_ = lives;
	};

	// Getter and setters for the x and y offset of the player's model.
	void SetOffset(float x, float y)
	{
		x_offset_ = x; y_offset_ = y;
	};

	float GetXOffset() {
		return x_offset_;
	};
	float GetYOffset() {
		return y_offset_;
	};

private:
	// Pointer to the platform.
	gef::Platform* platform_;

	// For holding the player's state.
	PlayerState player_state_;
	PlayerState old_player_state_;

	// Bools to hold the player's direction and whether it's falling.
	bool facing_left_;
	bool falling_;
	
	// The player model's offset.
	float x_offset_;
	float y_offset_;

	// Timer and time until the player respawns after dying.
	float timer_;
	float death_reset_time_;

	// The player's respawn position.
	b2Vec2 respawn_position_;

	// The player's lives remaining.
	int lives_;

	// The speed the player will travel at.
	float speed_;

	// For holding and creating the player's animated mesh.
	gef::Mesh* player_mesh_;
	gef::SkinnedMeshInstance* animated_mesh_;
	gef::Scene* player_scene_;

	// The player's animations and animation player.
	gef::Animation* kick_anim_;
	gef::Animation* idle_anim_;
	gef::Animation* jump_anim_;
	gef::Animation* fall_anim_;
	gef::Animation* land_anim_;
	gef::Animation* run_anim_;
	gef::Animation* death_anim_;
	gef::Animation* dance_anim_;
	MotionClipPlayer anim_player_;
};
