#pragma once
#include "game_object.h"
#include <graphics/skinned_mesh_instance.h>
#include <animation/animation.h>
#include <graphics/scene.h>
#include "motion_clip_player.h"
#include "graphics/renderer_3d.h"
#include "maths/math_utils.h"

// The three possible states that the enemy can have.
enum class EnemyState {
	IDLE,
	RUNNING,
	DEAD
};

// Direction enum to be used for launching the enemy when it is killed.
enum class Direction {
	UP, 
	DOWN, 
	LEFT,
	RIGHT
};

class Enemy :
	public GameObject
{
public:
	Enemy();

	// Functions for updating, initialising, rendering and reseting the enemy.
	void Update(float frame_time);
	void Init(gef::Platform* p, gef::Scene* s);
	void Render(gef::Renderer3D* renderer_3d);
	void Reset();
	
	// Function for setting the enemy as dead. Has a parameter for the direction that the kill came from.
	void SetDead(Direction dir);

	// Sets the distance that the enemy will travel, and the time it will remain idle for before switching directions.
	void SetPath(float distance, float time);

	// Getter and setters for the enemy's state.
	EnemyState GetState()
	{
		return enemy_state_;
	};

	void SetState(EnemyState state)
	{
		enemy_state_ = state;
	};

	// Getter and setters for the x and y offset of the enemy's model.
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

	// For holding the enemy's state.
	EnemyState enemy_state_;
	EnemyState old_enemy_state_;

	// Bool for the enemy's direction.
	bool facing_left_;

	// Enemy model's x and y offset.
	float x_offset_;
	float y_offset_;

	// Enemy's start position.
	b2Vec2 start_position_;

	// The distance the enemy will walk from its start position.
	float walk_distance_;

	// The speed the enemy will travel at.
	float speed_;

	// How long the enemy will idle for, and a timer to track the time passed.
	float idle_time_;
	float timer_;

	// The enemy's animated mesh.
	gef::SkinnedMeshInstance* animated_mesh_;

	// The enemy's animations and animation player.
	gef::Animation* idle_anim_;
	gef::Animation* run_anim_;
	MotionClipPlayer anim_player_;
};

