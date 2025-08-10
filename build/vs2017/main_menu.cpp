#include "main_menu.h"

MainMenu::MainMenu()
{
	// Set default values.
	music_playing_ = false;
	debug_ = false;
	selection_ = 0;
	controller_ = 1;
	lives_ = 5;
	controller_type_ = "NONE";
	button_pressed_ = false;
	settings_ = false;
	controls_ = false;
	active_touch_id_ = -1;
	touch_position_ = gef::Vector2(0.0f, 0.0f);
	old_mouse_position_ = gef::Vector2(0.0f, 0.0f);
	volume_ = 50.0f;
	timer_ = 0.0f;
	scroll_time_ = 0.2f;
}

void MainMenu::Update(float frame_time)
{
	// Increase timer by frame time.
	timer_ += frame_time;

	// Process input.
	if (input_manager_)
	{
		input_manager_->Update();
		ProcessTouchInput();
		ProcessKeyboardInput();
		ProcessControllerInput();
	}

	// Set controller string.
	if (controller_ == 0)
	{
		controller_type_ = "NONE";
	}
	else if (controller_ == 1)
	{
		controller_type_ = "XBOX";
	}
	else if (controller_ == 2)
	{
		controller_type_ = "SONY*";
	}

	// When a button is pressed...
	if (button_pressed_ == true)
	{
		audio_manager_->PlaySample(0); // Play a sound.

		switch (selection_) // Select what to do based on what button was pressed.
		{
		case 0:
			// Play - switch to level state and reset the level.
			game_state_->SetGameState(State::LEVEL);
			level_->Reset();
			break;
		case 1:
			// Settings - toggle settings window, make sure controls window is disabled.
			settings_ = !settings_;
			controls_ = false;
			selection_ = 4; // move selection to first button in settings.
			break;
		case 2:
			// Controls - toggle controls window, make sure settings window is disabled.
			controls_ = !controls_;
			settings_ = false;
			selection_ = 10; // move selection to back button.
			break;
		case 3:
			// Exit.
			exit(0);
			break;
		case 4:
			// Volume down.
			if (volume_ >= 10)
			{
				volume_ -= 10;
			}
			break;
		case 5:
			// Volume up.
			if (volume_ <= 90)
			{
				volume_ += 10;
			}
			break;
		case 6:
			// Gamepad left.
			if (controller_ > 0)
			{
				controller_ -= 1;
			}
			else
			{
				controller_ = 2;
			}
			break;
		case 7:
			// Gamepad right.
			if (controller_ < 2)
			{
				controller_ += 1;
			}
			else
			{
				controller_ = 0;
			}
			break;
		case 8:
			// Lives down.
			if (lives_ == 1)
			{
				lives_ = 999;
			}
			else if (lives_ == 5)
			{
				lives_ = 1;
			}
			else if (lives_ == 999)
			{
				lives_ = 5;
			}
			break;
		case 9:
			// Lives up.
			if (lives_ == 1)
			{
				lives_ = 5;
			}
			else if (lives_ == 5)
			{
				lives_ = 999;
			}
			else if (lives_ == 999)
			{
				lives_ = 1;
			}
			break;
		case 10:
			// Back.
			Back();
			break;
		default:
			break;
		}
		button_pressed_ = false;
	}

	// Start playing music when it isn't already playing.
	if (music_playing_ == false)
	{
		music_playing_ = true;
		audio_manager_->PlayMusic();
	}

	// Set the audio manager's volume.
	audio_manager_->SetMasterVolume(volume_);
}

void MainMenu::Render()
{
	// Render the menu.
	sprite_renderer_->Begin();

	// Draw the background image.
	sprite_renderer_->DrawSprite(background_image_);

	// Draw the game's title.
	sprite_renderer_->DrawSprite(title_);

	// Render the settings menu.
	RenderSettings();
	
	// Render the controls menu.
	RenderControls();

	// Render the buttons outline if in debug mode.
	if (debug_)
	{
		for (int i = 0; i < 4; i++)
		{
			sprite_renderer_->DrawSprite(menu_buttons_[i]);
		}
	}

	// Render the buttons.
	RenderButtons();

	sprite_renderer_->End();
}

void MainMenu::Init(gef::SpriteRenderer* sr, gef::Font* f, gef::Platform* p, GameState* gs, gef::InputManager* im, gef::AudioManager* am, Level* l)
{
	// Set pointers.
	sprite_renderer_ = sr;
	font_ = f;
	platform_ = p;
	game_state_ = gs;
	input_manager_ = im;
	audio_manager_ = am;
	level_ = l;

	// Load textures, assign them to sprites. Also set position and size.
	gef::ImageData texture_image;
	gef::PNGLoader png_loader;
	gef::Texture* texture;
	png_loader.Load("textures/background.png", *platform_, texture_image);
	texture = gef::Texture::Create(*platform_, texture_image);

	background_image_.set_texture(texture);
	background_image_.set_position(platform_->width() / 2, platform_->height() / 2, 0);
	background_image_.set_width(platform_->width());
	background_image_.set_height(platform_->height());

	png_loader.Load("textures/title.png", *platform_, texture_image);
	texture = gef::Texture::Create(*platform_, texture_image);
	title_.set_texture(texture);
	title_.set_position(platform_->width() * 0.5f, platform_->height() * 0.175f, 0.0f);
	title_.set_width(platform_->width() * 0.66);
	title_.set_height(platform_->height() * 0.33);

	settings_pane_.set_position(platform_->width() * 0.6, platform_->height() * 0.6, 0);
	settings_pane_.set_width(platform_->width() / 2);
	settings_pane_.set_height(platform_->height() / 2);

	png_loader.Load("textures/controls.png", *platform_, texture_image);
	texture = gef::Texture::Create(*platform_, texture_image);
	controls_pane_.set_texture(texture);
	controls_pane_.set_position(platform_->width() * 0.6, platform_->height() * 0.6, 0);
	controls_pane_.set_width(platform_->width() / 2);
	controls_pane_.set_height(platform_->height() / 2);

	// Setup audio.
	audio_manager_->SetMasterVolume(volume_); // start at 50% volume
	audio_manager_->LoadMusic("audio/masters_of_the_galaxy_symphonic.wav", *platform_);
	
	// Initialise the buttons.
	InitButtons();
}

void MainMenu::Reset()
{
	// Reset the music.
	music_playing_ = false;
	audio_manager_->LoadMusic("audio/masters_of_the_galaxy_symphonic.wav", *platform_);
}

void MainMenu::ProcessTouchInput()
{
	// Get touch input.
	const gef::TouchInputManager* touch_input = input_manager_->touch_manager();

	// Check to see if mouse moved so that you can change selection with keyboard/controller if you accidentally leave mouse over a button.
	if (old_mouse_position_.x != touch_input->mouse_position().x && old_mouse_position_.y != touch_input->mouse_position().y) 
	{
		// Highlighting selection with mouse.
		for (int i = 0; i < 4; i++)
		{
			if (IsInside(menu_buttons_[i], touch_input->mouse_position()))
			{
				selection_ = i;
			}
		}

		// Highlight settings selection.
		if (settings_)
		{
			for (int i = 4; i < 11; i++)
			{
				if (IsInside(menu_buttons_[i], touch_input->mouse_position()))
				{
					selection_ = i;
				}
			}
		}

		// Highlight back button.
		if (controls_)
		{
			if (IsInside(menu_buttons_[10], touch_input->mouse_position()))
			{
				selection_ = 10;
			}
		}
	}

	// Set old mouse position.
	old_mouse_position_ = touch_input->mouse_position();

	// Handle touch input.
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

					// Record touch position.
					touch_position_ = touch->position;
				}
			}
			else if (active_touch_id_ == touch->id)
			{
				// we are processing touch data with a matching id to the one we are looking for
				if (touch->type == gef::TT_ACTIVE)
				{
					// Record touch position.
					touch_position_ = touch->position;
				}
				else if (touch->type == gef::TT_RELEASED)
				{
					// the touch we are tracking has been released
					// perform any actions that need to happen when a touch is released here
					// we're not doing anything here apart from resetting the active touch id
					active_touch_id_ = -1;
					
					// When touch is released, activate selection if the mouse is over it.
					for (int i = 0; i < 4; i++)
					{
						if (IsInside(menu_buttons_[i], touch_position_))
						{
							selection_ = i;
							button_pressed_ = true;
						}
					}

					if (settings_)
					{
						for (int i = 4; i < 11; i++)
						{
							if (IsInside(menu_buttons_[i], touch_position_))
							{
								selection_ = i;
								button_pressed_ = true;
							}
						}
					}
					else if (controls_)
					{
						if (IsInside(menu_buttons_[10], touch_position_))
						{
							selection_ = 10;
							button_pressed_ = true;
						}
					}
				}
			}
		}
	}

	
}

void MainMenu::ProcessKeyboardInput()
{
	// Get keyboard input.
	gef::Keyboard* keyboard = input_manager_->keyboard();

	// Move menu down if S or down arrow is pressed.
	if (keyboard->IsKeyPressed(gef::Keyboard::KC_S) || keyboard->IsKeyPressed(gef::Keyboard::KC_DOWN))
	{
		SelectionDown();
	}

	// Move menu up if W or up arrow is pressed.
	if (keyboard->IsKeyPressed(gef::Keyboard::KC_W) || keyboard->IsKeyPressed(gef::Keyboard::KC_UP))
	{
		SelectionUp();
	}

	// Move menu left if A or left arrow is pressed.
	if (keyboard->IsKeyPressed(gef::Keyboard::KC_A) || keyboard->IsKeyPressed(gef::Keyboard::KC_LEFT))
	{
		SelectionLeft();
	}

	// Move menu right if D or right arrow is pressed.
	if (keyboard->IsKeyPressed(gef::Keyboard::KC_D) || keyboard->IsKeyPressed(gef::Keyboard::KC_RIGHT))
	{
		SelectionRight();
	}

	// Activate selection if enter, space or E is pressed.
	if (keyboard->IsKeyPressed(gef::Keyboard::KC_RETURN) || keyboard->IsKeyPressed(gef::Keyboard::KC_SPACE) || keyboard->IsKeyPressed(gef::Keyboard::KC_E))
	{
		button_pressed_ = true;
	}

	// Return if escape key is pressed.
	if (keyboard->IsKeyPressed(gef::Keyboard::KC_ESCAPE))
	{
		Back();
	}
}

void MainMenu::ProcessControllerInput()
{
	// Get controller input.
	gef::SonyControllerInputManager* controller_manager = input_manager_->controller_input();
	if (controller_manager)
	{
		// Get first controller.
		const gef::SonyController* controller = controller_manager->GetController(0);
		if (controller)
		{
			// Variables for tracking the left analogue stick.
			float left_x_ = controller->left_stick_x_axis();
			float left_y_ = controller->left_stick_y_axis();

			// When up on the d-pad is pressed or the analogue stick is up and the scroll timer exceeds scroll time, move selection up and reset timer.
			if (controller->buttons_pressed() & gef_SONY_CTRL_UP || (left_y_ < -0.66 && timer_ > scroll_time_))
			{
				SelectionUp();
				timer_ = 0;
			}

			// When down on the d-pad is pressed or the analogue stick is down and the scroll timer exceeds scroll time, move selection down and reset timer.
			if (controller->buttons_pressed() & gef_SONY_CTRL_DOWN || (left_y_ > 0.66 && timer_ > scroll_time_))
			{
				SelectionDown();
				timer_ = 0;
			}

			// When left on the d-pad is pressed or the analogue stick is left and the scroll timer exceeds scroll time, move selection left and reset timer.
			if (controller->buttons_pressed() & gef_SONY_CTRL_LEFT || (left_x_ < -0.66 && timer_ > scroll_time_))
			{
				SelectionLeft();
				timer_ = 0;
			}

			// When right on the d-pad is pressed or the analogue stick is right and the scroll timer exceeds scroll time, move selection right and reset timer.
			if (controller->buttons_pressed() & gef_SONY_CTRL_RIGHT || (left_x_ > 0.66 && timer_ > scroll_time_))
			{
				SelectionRight();
				timer_ = 0;
			}
			
			if (controller->buttons_pressed() & gef_SONY_CTRL_SQUARE && controller_ == 1) // Activate selection when A is pressed on Xbox controller.
			{
				button_pressed_ = true;
			}
			else if (controller->buttons_pressed() & gef_SONY_CTRL_CROSS && controller_ == 2) // Activate selection when X is pressed on Playstation controller.
			{
				button_pressed_ = true;
			}

			if (controller->buttons_pressed() & gef_SONY_CTRL_CROSS && controller_ == 1) // Return when B is pressed on Xbox controller.
			{
				Back();
			}
			else if (controller->buttons_pressed() & gef_SONY_CTRL_CIRCLE && controller_ == 2) // Return when circle is pressed on Playstation controller.
			{
				Back();
			}
		}
	}

}

void MainMenu::RenderButtons()
{
	// Highlight the selected button yellow, otherwise it's white.
	for (int i = 0; i < 4; i++)
	{
		if (i == selection_)
		{
			button_colour_[i] = 0xff00ffff;
		}
		else
		{
			button_colour_[i] = 0xffffffff;
		}
	}
	
	// Render each button's text.
	font_->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_->width() * 0.1f, platform_->height() * 0.4f, 0.0f),
		1.0f,
		button_colour_[0],
		gef::TJ_LEFT,
		"PLAY");

	font_->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_->width() * 0.1f, platform_->height() * 0.5f, 0.0f),
		1.0f,
		button_colour_[1],
		gef::TJ_LEFT,
		"SETTINGS");
	
	font_->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_->width() * 0.1f, platform_->height() * 0.6f, 0.0f),
		1.0f,
		button_colour_[2],
		gef::TJ_LEFT,
		"CONTROLS");
	
	font_->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_->width() * 0.1f, platform_->height() * 0.7f, 0.0f),
		1.0f,
		button_colour_[3],
		gef::TJ_LEFT,
		"EXIT");

}

void MainMenu::RenderSettings()
{
	if (settings_)
	{
		// Render settings pane if debug mode is enabled.
		if (debug_)
		{
			sprite_renderer_->DrawSprite(settings_pane_);
		}
		
		// Draw each settings button.
		for (int i = 4; i < 11; i++)
		{
			sprite_renderer_->DrawSprite(menu_buttons_[i]);
		}

		// Highlight selected button.
		for (int i = 4; i < 11; i++)
		{
			if (i == selection_)
			{
				button_colour_[i] = 0xff00ffff;
			}
			else
			{
				button_colour_[i] = 0xffffffff;
			}
		}

		// Render each buton's text.
		font_->RenderText(
			sprite_renderer_,
			gef::Vector4(platform_->width() * 0.375f, platform_->height() * 0.415, 0.0f),
			0.75f,
			0xffffffff,
			gef::TJ_LEFT,
			"VOLUME");

		font_->RenderText(
			sprite_renderer_,
			gef::Vector4(platform_->width() * 0.7f, platform_->height() * 0.415, 0.0f),
			0.75f,
			0xffffffff,
			gef::TJ_CENTRE,
			"%i%%",
			volume_);

		font_->RenderText(
			sprite_renderer_,
			gef::Vector4(platform_->width() * 0.595f, platform_->height() * 0.415, 0.0f),
			0.75f,
			button_colour_[4],
			gef::TJ_CENTRE,
			"<");

		font_->RenderText(
			sprite_renderer_,
			gef::Vector4(platform_->width() * 0.8f, platform_->height() * 0.415, 0.0f),
			0.75f,
			button_colour_[5],
			gef::TJ_CENTRE,
			">");

		font_->RenderText(
			sprite_renderer_,
			gef::Vector4(platform_->width() * 0.375f, platform_->height() * 0.515, 0.0f),
			0.75f,
			0xffffffff,
			gef::TJ_LEFT,
			"GAMEPAD");

		font_->RenderText(
			sprite_renderer_,
			gef::Vector4(platform_->width() * 0.7f, platform_->height() * 0.515, 0.0f),
			0.75f,
			0xffffffff,
			gef::TJ_CENTRE,
			"%s",
			controller_type_.c_str());

		font_->RenderText(
			sprite_renderer_,
			gef::Vector4(platform_->width() * 0.595f, platform_->height() * 0.515, 0.0f),
			0.75f,
			button_colour_[6],
			gef::TJ_CENTRE,
			"<");

		font_->RenderText(
			sprite_renderer_,
			gef::Vector4(platform_->width() * 0.8f, platform_->height() * 0.515, 0.0f),
			0.75f,
			button_colour_[7],
			gef::TJ_CENTRE,
			">");

		font_->RenderText(
			sprite_renderer_,
			gef::Vector4(platform_->width() * 0.375f, platform_->height() * 0.615, 0.0f),
			0.75f,
			0xffffffff,
			gef::TJ_LEFT,
			"LIVES");

		font_->RenderText(
			sprite_renderer_,
			gef::Vector4(platform_->width() * 0.7f, platform_->height() * 0.615, 0.0f),
			0.75f,
			0xffffffff,
			gef::TJ_CENTRE,
			"%i",
			lives_);

		font_->RenderText(
			sprite_renderer_,
			gef::Vector4(platform_->width() * 0.595f, platform_->height() * 0.615, 0.0f),
			0.75f,
			button_colour_[8],
			gef::TJ_CENTRE,
			"<");

		font_->RenderText(
			sprite_renderer_,
			gef::Vector4(platform_->width() * 0.8f, platform_->height() * 0.615, 0.0f),
			0.75f,
			button_colour_[9],
			gef::TJ_CENTRE,
			">");

		font_->RenderText(
			sprite_renderer_,
			gef::Vector4(platform_->width() * 0.775f, platform_->height() * 0.765, 0.0f),
			0.75f,
			button_colour_[10],
			gef::TJ_CENTRE,
			"BACK");

		if (controller_ == 2)
		{
			font_->RenderText(
				sprite_renderer_,
				gef::Vector4(platform_->width() * 0.375f, platform_->height() * 0.765, 0.0f),
				0.75f,
				0xffffffff,
				gef::TJ_LEFT,
				"*untested");
		}
	}
}

void MainMenu::RenderControls()
{
	if (controls_)
	{
		// Draw the controls image.
		sprite_renderer_->DrawSprite(controls_pane_);
		
		// Render back button.
		sprite_renderer_->DrawSprite(menu_buttons_[10]);

		// Highlight selection.
		if (selection_ == 10)
		{
			button_colour_[10] = 0xff00ffff;
		}
		else
		{
			button_colour_[10] = 0xffffffff;
		}

		// Back button text.
		font_->RenderText(
			sprite_renderer_,
			gef::Vector4(platform_->width() * 0.775f, platform_->height() * 0.765, 0.0f),
			0.75f,
			button_colour_[10],
			gef::TJ_CENTRE,
			"BACK");
	}
}

void MainMenu::InitButtons()
{
	// Initialise each buttons size, position and colour.
	for (int i = 0; i < 4; i++)
	{
		menu_buttons_[i].set_height(50);
		menu_buttons_[i].set_width(200);
		menu_buttons_[i].set_position(platform_->width() * 0.205f, platform_->height() * (0.45 + (0.1 * i)), 0);
	}

	for (int i = 4; i < 11; i++)
	{
		if (i == 10)
		{
			menu_buttons_[i].set_height(50);
			menu_buttons_[i].set_width(100);
			menu_buttons_[i].set_colour(0xff000000);
			menu_buttons_[i].set_position(platform_->width() * 0.775f, platform_->height() * (0.5 + (0.05 * (i - 4))), 0);
		}
		else
		{
			menu_buttons_[i].set_height(50);
			menu_buttons_[i].set_width(50);
			menu_buttons_[i].set_colour(0xff000000);

			if (i % 2 == 0)
			{
				menu_buttons_[i].set_position(platform_->width() * 0.6f, platform_->height() * (0.45 + (0.05 * (i - 4))), 0);
			}
			else
			{
				menu_buttons_[i].set_position(platform_->width() * 0.8f, platform_->height() * (0.45 + (0.05 * (i - 5))), 0);
			}
		}
	}

	for (int i = 0; i < 11; i++)
	{
		if (i < 4)
		{
			button_colour_[i] = (0xffffffff);
		}
		else
		{
			button_colour_[i] = (0xff000000);
		}
	}
}

bool MainMenu::IsInside(const gef::Sprite& sprite, const gef::Vector2& point)
{
	// Return true if the point is within all of the sprite's boundaries.
	if (point.x < sprite.position().x() - sprite.width() / 2)
		return false;
	if (point.x > sprite.position().x() + sprite.width() / 2)
		return false;
	if (point.y < sprite.position().y() - sprite.height() / 2)
		return false;
	if (point.y > sprite.position().y() + sprite.height() / 2)
		return false;

	return true;
}

void MainMenu::SelectionUp()
{
	// Increase settings integer based on current menu status.
	if (!settings_ && !controls_)
	{
		selection_ -= 1;
		if (selection_ < 0)
		{
			selection_ = 3;
		}
	}
	else if (settings_)
	{
		if (selection_ == 10)
		{
			selection_ -= 1;
		}
		else
		{
			selection_ -= 2;
		}

		if (selection_ < 4)  // If they escape settings menu back into main menu, return to settings.
		{
			selection_ = 4;
		}
	}
}

void MainMenu::SelectionDown()
{
	// Decrease settings integer based on current menu status.
	if (!settings_ && !controls_)
	{
		selection_ += 1;
		if (selection_ > 3)
		{
			selection_ = 0;
		}
	}
	else if (settings_)
	{
		selection_ += 2;
		if (selection_ > 10)
		{
			selection_ = 10;
		}
		else if (selection_ < 4) // If they escape settings menu back into main menu, return to settings.
		{
			selection_ = 4;
		}
	}
}

void MainMenu::SelectionLeft()
{
	// Move selection left when at settings menu.
	if (settings_ && selection_ != 10)
	{
		if (selection_ % 2 == 0)
		{
			button_pressed_ = true; // Allows you to control volume, gamepad selection, etc by pressing left when at left button.
		}
		else
		{
			selection_ -= 1;
		}

		if (selection_ < 4) // If they escape settings menu back into main menu, return to settings.
		{
			selection_ = 10;
		}
	}
}

void MainMenu::SelectionRight()
{
	// Move selection right when at settings menu.
	if (settings_ && selection_ != 10)
	{
		if (selection_ % 2 == 0)
		{
			selection_ += 1;
		}
		else
		{
			button_pressed_ = true; // Allows you to control volume, gamepad selection, etc by pressing right when at right button.
		}

		if (selection_ > 10)
		{
			selection_ = 4;
		}
		else if (selection_ < 4) // If they escape settings menu back into main menu, return to settings.
		{
			selection_ = 4;
		}
	}
}

void MainMenu::Back()
{
	// Disable settings and controls, return to first selection in the menu.
	if (settings_ || controls_)
	{
		settings_ = false;
		controls_ = false;
		selection_ = 0;
	}
}
