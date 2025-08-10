#include "end_screen.h"

// Constructor
EndScreen::EndScreen()
{
	active_touch_id_ = -1; // Not currently processing a touch.
}

void EndScreen::Update(float frame_time)
{
	// Update the input manager, then call the functions for processing each kind of input.
	if (input_manager_)
	{
		input_manager_->Update();
		ProcessTouchInput();
		ProcessKeyboardInput();
		ProcessControllerInput();
	}
}

void EndScreen::Render()
{
	// Start rendering
	sprite_renderer_->Begin();

	// Select which background image to render.
	if (game_state_->GetGameState() == State::WIN)
	{
		sprite_renderer_->DrawSprite(win_image_);
	}
	else if (game_state_->GetGameState() == State::LOSE)
	{
		sprite_renderer_->DrawSprite(lose_image_);
	}
	
	// If the player wins, display 'level complete', the amount of coins they collected and the time it took them.
	if (game_state_->GetGameState() == State::WIN)
	{
		font_->RenderText(
			sprite_renderer_,
			gef::Vector4(platform_->width() * 0.5f, platform_->height() * 0.1f, 0.0f),
			2.0f,
			0xffffffff,
			gef::TJ_CENTRE,
			"LEVEL COMPLETE");

		font_->RenderText(
			sprite_renderer_,
			gef::Vector4(platform_->width() * 0.05f, platform_->height() * 0.4f, 0.0f),
			1.0f,
			0xffffffff,
			gef::TJ_LEFT,
			"COINS COLLECTED: %i/100",
			level_->GetScore());

		font_->RenderText(
			sprite_renderer_,
			gef::Vector4(platform_->width() * 0.05f, platform_->height() * 0.5f, 0.0f),
			1.0f,
			0xffffffff,
			gef::TJ_LEFT,
			"TIME: %.1fs",
			level_->GetTime());
	}
	else if (game_state_->GetGameState() == State::LOSE) // if the player loses, display 'you lose'.
	{
		font_->RenderText(
			sprite_renderer_,
			gef::Vector4(platform_->width() * 0.5f, platform_->height() * 0.1f, 0.0f),
			2.0f,
			0xffffffff,
			gef::TJ_CENTRE,
			"YOU LOSE");
	}
	
	// Inform the user they can press any button to return to the main menu.
	font_->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_->width() * 0.5f, platform_->height() * 0.8f, 0.0f),
		1.0f,
		0xffffffff,
		gef::TJ_CENTRE,
		"PRESS ANY BUTTON TO RETURN TO MAIN MENU");

	// End rendering
	sprite_renderer_->End();
}

void EndScreen::Init(gef::SpriteRenderer* sr, gef::Font* f, gef::Platform* p, GameState* gs, gef::InputManager* im, gef::AudioManager* am, Level* l, MainMenu* mm)
{
	// Assign values to all of the pointers.
	sprite_renderer_ = sr;
	font_ = f;
	platform_ = p;
	game_state_ = gs;
	input_manager_ = im;
	audio_manager_ = am;
	level_ = l;
	main_menu_ = mm;

	// Load and asssign the textures for the win and lose background images.
	gef::ImageData texture_image;
	gef::PNGLoader png_loader;
	png_loader.Load("textures/win.png", *platform_, texture_image);
	gef::Texture* texture = gef::Texture::Create(*platform_, texture_image);

	win_image_.set_texture(texture);
	win_image_.set_position(platform_->width() / 2, platform_->height() / 2, 0);
	win_image_.set_width(platform_->width());
	win_image_.set_height(platform_->height());

	png_loader.Load("textures/lose.png", *platform_, texture_image);
	texture = gef::Texture::Create(*platform_, texture_image);
	lose_image_.set_texture(texture);
	lose_image_.set_position(platform_->width() / 2, platform_->height() / 2, 0);
	lose_image_.set_width(platform_->width());
	lose_image_.set_height(platform_->height());
}

void EndScreen::ProcessTouchInput()
{
	// Get the touch manager from input manager.
	const gef::TouchInputManager* touch_input = input_manager_->touch_manager();

	if (touch_input && (touch_input->max_num_panels() > 0))
	{
		// get the active touches for this panel
		const gef::TouchContainer& panel_touches = touch_input->touches(0);

		// go through the touches
		for (gef::ConstTouchIterator touch = panel_touches.begin(); touch != panel_touches.end(); ++touch)
		{
			// if active touch id is -1, then we are not currently processing a touch
			if (active_touch_id_ == -1)
			{
				// check for the start of a new touch
				if (touch->type == gef::TT_NEW)
				{
					active_touch_id_ = touch->id;
				}
			}
			else if (active_touch_id_ == touch->id)
			{
				// we are processing touch data with a matching id to the one we are looking for
				if (touch->type == gef::TT_ACTIVE)
				{
					// do nothing
				}
				else if (touch->type == gef::TT_RELEASED)
				{	
					// Returns to the menu once the mouse click has been released.
					ReturnToMenu();
				}
			}
		}
	}

}

void EndScreen::ProcessKeyboardInput()
{
	// Get the keyboard input from input manager.
	gef::Keyboard* keyboard = input_manager_->keyboard();

	// If any key is pressed, return to the main menu.
	for (int i = 0; i < gef::Keyboard::NUM_KEY_CODES; i++)
	{
		if (keyboard->IsKeyPressed(gef::Keyboard::KeyCode(i)))
		{
			ReturnToMenu();
		}
	}
	

}

void EndScreen::ProcessControllerInput()
{
	// Get the controller manager from input manager.
	gef::SonyControllerInputManager* controller_manager = input_manager_->controller_input();
	if (controller_manager)
	{
		// Get the current controller.
		const gef::SonyController* controller = controller_manager->GetController(0);
		if (controller)
		{
			// If any button is pressed, return to the main menu.
			if (controller->buttons_pressed() != 0)
			{
				ReturnToMenu();
			}
		}
	}
}

void EndScreen::ReturnToMenu()
{
	// Reset the main menu, then switch to the menu state.
	main_menu_->Reset();
	game_state_->SetGameState(State::MENU);
}
