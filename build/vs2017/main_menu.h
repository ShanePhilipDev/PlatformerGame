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

class Level;

class MainMenu
{
public:
	MainMenu();

	// Functions for updating, rendering, initialising and reseting the main menu.
	void Update(float frame_time);
	void Render();
	void Init(gef::SpriteRenderer* sr, gef::Font* f, gef::Platform* p, GameState* gs, gef::InputManager* im, gef::AudioManager* am, Level* l);
	void Reset();

	// Functions to retrieve the settings from the menu in the level.
	int* GetController() {
		return &controller_;
	};
	int* GetVolume() {
		return &volume_;
	};
	int GetLives() {
		return lives_;
	};
private:
	// Functions for processing the input.
	void ProcessTouchInput();
	void ProcessKeyboardInput();
	void ProcessControllerInput();

	// Functions for rendering elements of the menu.
	void RenderButtons();
	void RenderSettings();
	void RenderControls();

	// Function for initialising the buttons.
	void InitButtons();
	
	// Functions for navigating the menu.
	void SelectionUp();
	void SelectionDown();
	void SelectionLeft();
	void SelectionRight();
	void Back();

	// Function for checking if the mouse cursor is inside the button.
	bool IsInside(const gef::Sprite& sprite, const gef::Vector2& point);

	// Main menu's pointers.
	gef::SpriteRenderer* sprite_renderer_;
	gef::Font* font_;
	gef::Platform* platform_;
	gef::InputManager* input_manager_;
	gef::AudioManager* audio_manager_;
	GameState* game_state_; 
	Level* level_;

	// For controlling the speed that the menu scrolls at.
	float timer_;
	float scroll_time_;

	// The sprites needed to construct the menu.
	gef::Sprite background_image_;
	gef::Sprite settings_pane_;
	gef::Sprite controls_pane_;
	gef::Sprite title_;
	gef::Sprite menu_buttons_[11];

	// Holds the colour of the corresponding button.
	UInt32 button_colour_[11];

	// Store the old mouse position to check if the mouse position has changed.
	gef::Vector2 old_mouse_position_;

	// The current menu selection.
	int selection_;

	// The controller/gamepad. 0 = none, 1 = xbox, 2 = sony.
	int controller_;

	// The lives that the player will have in the level. Choose between 1, 5 and 999.
	int lives_;

	// String for displaying the current controller.
	std::string controller_type_;

	// Adjustable volume value.
	int volume_;

	// Variables for tracking touches.
	Int32 active_touch_id_;
	gef::Vector2 touch_position_;

	// Bool for whether the music has started playing.
	bool music_playing_;

	// Bool for toggling rendering the button's hitboxes.
	bool debug_;

	// Bools for enabling the settings and controls parts of the menu.
	bool settings_;
	bool controls_;

	// Bool for tracking if the user has pressed the button.
	bool button_pressed_;
};

