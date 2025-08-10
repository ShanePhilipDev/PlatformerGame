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
#include "level.h"
#include "main_menu.h"

class EndScreen
{
public:
	EndScreen();

	// Functions for updating, rendering and initialising.
	void Update(float frame_time);
	void Render();
	void Init(gef::SpriteRenderer* sr, gef::Font* f, gef::Platform* p, GameState* gs, gef::InputManager* im, gef::AudioManager* am, Level* l, MainMenu* mm);

private:
	// Functions for processing input.
	void ProcessTouchInput();
	void ProcessKeyboardInput();
	void ProcessControllerInput();

	// Function to switch to the main menu.
	void ReturnToMenu();

	// Pointers that the menu requires.
	gef::SpriteRenderer* sprite_renderer_;
	gef::Font* font_;
	gef::Platform* platform_;
	gef::InputManager* input_manager_;
	gef::AudioManager* audio_manager_;
	GameState* game_state_;
	Level* level_;
	MainMenu* main_menu_;

	// Background images for the win and lose state.
	gef::Sprite win_image_;
	gef::Sprite lose_image_;

	// Store the current touch id.
	Int32 active_touch_id_;
};

