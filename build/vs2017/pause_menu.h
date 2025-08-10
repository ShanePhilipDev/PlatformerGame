#pragma once
#include "main_menu.h"
class PauseMenu
{
public:
	PauseMenu();

	// Functions for updating, rendering, and initialising the pause menu.
	void Update(float frame_time);
	void Render();
	void Init(gef::SpriteRenderer* sr, gef::Font* f, gef::Platform* p, GameState* gs, gef::InputManager* im, gef::AudioManager* am, Level* l, MainMenu* mm);
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
	
	// Function for checking if the mouse cursor is inside the button.
	bool IsInside(const gef::Sprite& sprite, const gef::Vector2& point);
	
	// Functions for navigating the menu.
	void SelectionUp();
	void SelectionDown();
	void SelectionLeft();
	void SelectionRight();
	void Back();

	// Pause menu's pointers.
	gef::SpriteRenderer* sprite_renderer_;
	gef::Font* font_;
	gef::Platform* platform_;
	gef::InputManager* input_manager_;
	gef::AudioManager* audio_manager_;
	GameState* game_state_;
	Level* level_;
	MainMenu* main_menu_;

	// For controlling the speed that the menu scrolls at.
	float timer_;
	float scroll_time_;

	// The sprites needed to construct the menu.
	gef::Sprite settings_pane_;
	gef::Sprite controls_pane_;
	gef::Sprite menu_buttons_[11];

	// Holds the colour of the corresponding button.
	UInt32 button_colour_[11];

	// Store the old mouse position to check if the mouse position has changed.
	gef::Vector2 old_mouse_position_;

	// The current menu selection.
	int selection_;

	// Pointer to the controller/gamepad. 0 = none, 1 = xbox, 2 = sony.
	int* controller_;

	// String for displaying the current controller.
	std::string controller_type_;

	// Pointer to the volume value.
	int* volume_;

	// Variables for tracking touches.
	Int32 active_touch_id_;
	gef::Vector2 touch_position_;

	// Bool for toggling rendering the button's hitboxes.
	bool debug_;

	// Bools for enabling the settings and controls parts of the menu.
	bool settings_;
	bool controls_;

	// Bool for tracking if the user has pressed the button.
	bool button_pressed_;
};

