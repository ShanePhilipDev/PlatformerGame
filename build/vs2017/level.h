#pragma once
#include "game_state.h"
#include "gef.h"
#include "graphics/sprite_renderer.h"
#include "graphics/font.h"
#include "system/platform.h"
#include "graphics/sprite.h"
#include "graphics/texture.h"
#include "graphics/image_data.h"
#include "assets/png_loader.h"
#include "audio/audio_manager.h"
#include "input/input_manager.h"
#include "input/touch_input_manager.h"
#include <input/sony_controller_input_manager.h>
#include <input/keyboard.h>
#include "main_menu.h"
#include <primitive_builder.h>
#include <box2d/Box2D.h>
#include "game_object.h"
#include "graphics/default_3d_shader.h"
#include "graphics/point_light.h"
#include "graphics/renderer_3d.h"
#include "maths/math_utils.h"
#include "player.h"
#include <graphics/skinned_mesh_instance.h>
#include "enemy.h"
#include "crate.h"
#include "coin.h"
#include "sawblade.h"
#include "crusher.h"
#include "checkpoint.h"

class MainMenu;

class Level
{
public:
	Level();

	// Functions for updating, rendering, initialising and reseting the level.
	void Update(float frame_time);
	void Render();
	void Init(gef::SpriteRenderer* sr, gef::Font* f, gef::Platform* p, GameState* gs, gef::InputManager* im, gef::AudioManager* am, MainMenu* mm, gef::Renderer3D* r3d, PrimitiveBuilder* pb);
	void Reset();

	// Getters for the score and time of the level, to be used in the end screen.
	int GetScore()
	{
		return score_;
	};
	float GetTime()
	{
		return timer_;
	};

private:
	// Functions for processing input.
	void ProcessTouchInput();
	void ProcessKeyboardInput(float frame_time);
	void ProcessControllerInput(float frame_time);

	// Functions for initialising each of the objects in the world.
	void InitPlayer();
	void InitEnemies();
	void InitGround();
	void InitLights();
	void InitTextures();
	void InitCrates();
	void InitWall();
	void InitCoins();
	void InitTraps();
	void InitCheckpoints();

	// Function for the box2d physics simulation.
	void UpdateSimulation(float frame_time);

	// Function for rendering the hud.
	void RenderHud();

	// Pointers that the level needs.
	gef::SpriteRenderer* sprite_renderer_;
	gef::Font* font_;
	gef::Platform* platform_;
	gef::InputManager* input_manager_;
	gef::AudioManager* audio_manager_;
	GameState* game_state_;
	gef::Renderer3D* renderer_3d_;
	PrimitiveBuilder* primitive_builder_;
	MainMenu* main_menu_;
	b2World* world_;
	int* volume_;
	int* controller_;

	// Bool for whether music is playing.
	bool music_playing_;

	// The lives of the player and the score (coins collected).
	int lives_;
	int score_;

	// Timer for tracking how long the level has been played for, and a timer for the time to switch state after finishing the level.
	float timer_;
	float end_timer_;

	// Distance for audio to be heard by player for the crushers.
	float audio_proximity_;

	// Player, crate and crusher's half heights, used in collisions.
	float player_half_height_;
	float crate_half_height_;
	float crusher_half_height_;

	// For handling touch input.
	Int32 active_touch_id_;

	// The player's respawn position, changes when checkpoints are activated.
	b2Vec2 respawn_position_;

	// Bool for switching between each footstep sound.
	bool alternate_footsteps_;

	// For controlling the time between each footstep.
	float footstep_timer_;

	// The materials for the objects in the world.
	gef::Material floor_material_;
	gef::Material crate__material_;
	gef::Material jump_crate_material_;
	gef::Material metal_crate_material_;
	gef::Material metal_jump_crate_material_;
	gef::Material metal_material_;
	gef::Material wall_material_;
	gef::Material wood_material_;
	gef::Material coin_material_;
	gef::Material sawblade_material_;
	gef::Material checkpoint_material_;
	
	// The player.
	Player player_;

	// Objects that make up the world. Arrays used as there is a fixed amount of them.
	// Enemies.
	const int enemy_count_ = 7;
	Enemy enemies_[7];
	
	// Crates.
	const int crate_count_ = 40;
	Crate crates_[40];
	
	// Coins.
	const int coin_count_ = 37;
	Coin coins_[37];
	
	// Sawblades.
	const int sawblade_count_ = 6;
	Sawblade sawblades_[6];
	
	// Crushers.
	const int crusher_count_ = 7;
	Crusher crushers_[7];

	// Walls.
	const int wall_count_ = 31;
	GameObject wall_[31];
	
	// The ground.
	const int ground_count_ = 10;
	GameObject ground_[10];
	
	// Checkpoints.
	const int checkpoint_count_ = 4;
	Checkpoint checkpoints_[4];
};

