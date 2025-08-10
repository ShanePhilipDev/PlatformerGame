#include "scene_app.h"
#include <system/platform.h>
#include <graphics/sprite_renderer.h>
#include <graphics/font.h>
#include <system/debug_log.h>
#include <graphics/renderer_3d.h>
#include <graphics/mesh.h>
#include <maths/math_utils.h>
#include <input/sony_controller_input_manager.h>
#include <graphics/sprite.h>
#include "load_texture.h"

SceneApp::SceneApp(gef::Platform& platform) :
	Application(platform),
	sprite_renderer_(NULL),
	renderer_3d_(NULL),
	primitive_builder_(NULL),
	input_manager_(NULL),
	font_(NULL),
	world_(NULL),
	audio_manager_(NULL)
{
}

void SceneApp::Init()
{
	// Create the sprite renderer.
	sprite_renderer_ = gef::SpriteRenderer::Create(platform_);

	// Create the input manager.
	input_manager_ = gef::InputManager::Create(platform_);

	if (input_manager_ && input_manager_->touch_manager() && (input_manager_->touch_manager()->max_num_panels() > 0))
	{
		input_manager_->touch_manager()->EnablePanel(0);
	}
		
	// Create the audio manager.
	audio_manager_ = gef::AudioManager::Create();

	// Create the 3D renderer.
	renderer_3d_ = gef::Renderer3D::Create(platform_);

	// Create the primitive builder.
	primitive_builder_ = new PrimitiveBuilder(platform_);

	// Create and load in the font.
	font_ = new gef::Font(platform_);
	font_->Load("fonts/font");

	// Set the initial game state to be the splash screen.
	game_state_.SetGameState(State::SPLASH);

	// Load all of the sounds.
	InitSounds();

	// Creates objects for each of the states and passes through the relevant pointers as arguments.
	splash_.Init(sprite_renderer_, font_, &platform_, &game_state_);
	main_menu_.Init(sprite_renderer_, font_, &platform_, &game_state_, input_manager_, audio_manager_, &level_);
	level_.Init(sprite_renderer_, font_, &platform_, &game_state_, input_manager_, audio_manager_, &main_menu_, renderer_3d_, primitive_builder_);
	pause_menu_.Init(sprite_renderer_, font_, &platform_, &game_state_, input_manager_, audio_manager_, &level_, &main_menu_);
	end_screen_.Init(sprite_renderer_, font_, &platform_, &game_state_, input_manager_, audio_manager_, &level_, &main_menu_);
}

void SceneApp::CleanUp()
{
	// Delete all pointers and set as null.
	delete input_manager_;
	input_manager_ = NULL;

	delete font_;
	font_ = NULL;

	delete sprite_renderer_;
	sprite_renderer_ = NULL;

	delete primitive_builder_;
	primitive_builder_ = NULL;

	delete renderer_3d_;
	renderer_3d_ = NULL;

	delete audio_manager_;
	audio_manager_ = NULL;

	delete world_;
	world_ = NULL;
}

bool SceneApp::Update(float frame_time)
{
	// Call an update function based on the current state.
	switch (game_state_.GetGameState())
	{
	case State::SPLASH:
		splash_.Update(frame_time);
		break;
	case State::MENU:
		main_menu_.Update(frame_time);
		break;
	case State::PAUSED:
		pause_menu_.Update(frame_time);
		break;
	case State::LEVEL:
		level_.Update(frame_time);
		break;
	case State::WIN:
		end_screen_.Update(frame_time);
		break;
	case State::LOSE:
		end_screen_.Update(frame_time);
		break;
	default:
		break;
	}


	return true;
}

void SceneApp::Render()
{
	// Call a render function based on the current state.
	switch (game_state_.GetGameState())
	{
	case State::SPLASH:
		splash_.Render();
		break;
	case State::MENU:
		main_menu_.Render();
		break;
	case State::PAUSED:
		pause_menu_.Render();
		break;
	case State::LEVEL:
		level_.Render();
		break;
	case State::WIN:
		end_screen_.Render();
		break;
	case State::LOSE:
		end_screen_.Render();
		break;
	default:
		break;
	}
}

void SceneApp::InitSounds()
{
	// Load all of the audio files.
	audio_manager_->LoadSample("audio/button_click.wav", platform_); // 0 - Button Click
	audio_manager_->LoadSample("audio/spin_kick.wav", platform_); // 1 - Spinning Kick
	audio_manager_->LoadSample("audio/bounce.wav", platform_); // 2 - Bounce
	audio_manager_->LoadSample("audio/crate_break.wav", platform_); // 3 - Crate Break
	audio_manager_->LoadSample("audio/enemy_hit.wav", platform_); // 4 - Enemy Hit
	audio_manager_->LoadSample("audio/scream.wav", platform_); // 5 - Scream
	audio_manager_->LoadSample("audio/footstep1.wav", platform_); // 6 - Footstep 1
	audio_manager_->LoadSample("audio/footstep2.wav", platform_); // 7 - Footstep 2
	audio_manager_->LoadSample("audio/coin.wav", platform_); // 8 - Coin Collected
	audio_manager_->LoadSample("audio/clang.wav", platform_); // 9 - Metallic Clang
}

