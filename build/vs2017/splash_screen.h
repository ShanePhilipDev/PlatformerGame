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

class SplashScreen
{
public:
	SplashScreen();

	// Functions for updaying, rendering and initialising the splash screen.
	void Update(float frame_time);
	void Render();
	void Init(gef::SpriteRenderer* sr, gef::Font* f, gef::Platform* p, GameState* gs);
private:
	// Required pointers for the splash screen.
	gef::SpriteRenderer* sprite_renderer_;
	gef::Font* font_;
	gef::Platform* platform_;
	GameState* game_state_;

	// Floats for timing the splash screen.
	float timer_;
	float splash_time_;

	// Sprite to store the splash screen's image.
	gef::Sprite splash_image_;
};

