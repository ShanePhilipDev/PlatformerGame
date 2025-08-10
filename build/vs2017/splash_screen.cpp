#include "splash_screen.h"

// Constructor
SplashScreen::SplashScreen()
{
	timer_ = 0.0f;
	splash_time_ = 2.0f; // The splash screen will last for 2 seconds.
}

void SplashScreen::Update(float frame_time)
{
	timer_ += frame_time;
	if (timer_ > splash_time_) // When the timer exceeds the splash screen's intended time, switch to the main menu.
	{
		timer_ = 0.0f;
		game_state_->SetGameState(State::MENU);
	}
}

// Render the splash image.
void SplashScreen::Render()
{
	sprite_renderer_->Begin();
	
	sprite_renderer_->DrawSprite(splash_image_);

	sprite_renderer_->End();
}

void SplashScreen::Init(gef::SpriteRenderer* sr, gef::Font* f, gef::Platform* p, GameState* gs)
{
	// Assign pointers.
	sprite_renderer_ = sr;
	font_ = f;
	platform_ = p;
	game_state_ = gs;

	// Create a texture for the splash image and assign it to the sprite.
	gef::ImageData texture_image;
	gef::PNGLoader png_loader;
	png_loader.Load("textures/splash.png", *platform_, texture_image);
	gef::Texture* texture = gef::Texture::Create(*platform_, texture_image);

	splash_image_.set_texture(texture);
	splash_image_.set_position(platform_->width() / 2, platform_->height() / 2, 0);
	splash_image_.set_width(platform_->width());
	splash_image_.set_height(platform_->height());
}
