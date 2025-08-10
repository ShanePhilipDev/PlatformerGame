#ifndef _SCENE_APP_H
#define _SCENE_APP_H

#include <system/application.h>
#include <maths/vector2.h>
#include "primitive_builder.h"
#include <graphics/mesh_instance.h>
#include <input/input_manager.h>
#include <box2d/Box2D.h>
#include "game_object.h"
#include "main_menu.h"
#include "pause_menu.h"
#include "splash_screen.h"
#include "game_state.h"
#include "level.h"
#include "end_screen.h"


// FRAMEWORK FORWARD DECLARATIONS
namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class InputManager;
	class Renderer3D;
}

class SceneApp : public gef::Application
{
public:
	SceneApp(gef::Platform& platform);

	// Function for initialising the main objects used in the game.
	void Init();

	// Function to clean up pointers.
	void CleanUp();

	// Main functions for updating and rendering the game.
	bool Update(float frame_time);
	void Render();
private:
	// Function to load sounds.
	void InitSounds();

	// The main pointers needed for the game.
	gef::SpriteRenderer* sprite_renderer_;
	gef::Font* font_;
	gef::InputManager* input_manager_;
	gef::AudioManager* audio_manager_;
	gef::Renderer3D* renderer_3d_;
	PrimitiveBuilder* primitive_builder_;
	b2World* world_;

	// Holds the current game state.
	GameState game_state_;

	// Objects for each of the states.
	SplashScreen splash_;
	MainMenu main_menu_;
	PauseMenu pause_menu_;
	Level level_;
	EndScreen end_screen_;
};

#endif // _SCENE_APP_H
