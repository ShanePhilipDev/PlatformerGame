#include "level.h"

Level::Level()
{
	// Set default values.
	music_playing_ = false;
	score_ = 0;
	lives_ = 3;
	alternate_footsteps_ = false;
	footstep_timer_ = 0.0f;
	timer_ = 0.0f;
	end_timer_ = 0.0f;
	respawn_position_ = b2Vec2(-8.0f, 3.0f);
	active_touch_id_ = -1;
	audio_proximity_ = 15.0f;
}

void Level::Update(float frame_time)
{
	// If finish line hasn't been reached, increase timer by frame time.
	if (!checkpoints_[3].GetTriggered()) 
	{
		timer_ += frame_time;
	}
	
	// Increase footstep timer.
	footstep_timer_ += frame_time;

	// If the player runs out of lives, switch to the lose state.
	if (player_.GetLives() < 1)
	{
		game_state_->SetGameState(State::LOSE);
	}

	// If the player falls out of the map, set them to be dead.
	if (player_.GetBody()->GetPosition().y < 0 && player_.GetState() != PlayerState::DEAD)
	{
		player_.SetDead();
		audio_manager_->PlaySample(5); // Play death sound.
	}

	// Iteratre through each checkpoint.
	for (int i = 0; i < checkpoint_count_; i++)
	{
		// If the checkpoint is triggered...
		if (checkpoints_[i].GetTriggered())
		{
			// If it's the last checkpoint, start increasing the end timer, start dancing and once the timer exceeds 5 seconds change to the win state.
			if (i == 3)
			{
				end_timer_ += frame_time;
				if (end_timer_ > 5)
				{
					game_state_->SetGameState(State::WIN);
				}
				player_.SetState(PlayerState::DANCING);
			}
			// Adjust respawn position to be equal to the checkpoint's position.
			respawn_position_ = checkpoints_[i].GetPosition();
			player_.SetRespawnPosition(respawn_position_);
		}
	}

	// Handle input.
	if (input_manager_)
	{
		input_manager_->Update();
		ProcessTouchInput();
		ProcessKeyboardInput(frame_time);
		ProcessControllerInput(frame_time);
	}

	// When the player is running, play footstep sounds.
	if (player_.GetState() == PlayerState::RUNNING)
	{
		// When the timer exceeds 0.3...
		if (footstep_timer_ > 0.3) // 0.3 seconds between each footstep.
		{
			// Reset the timer.
			footstep_timer_ = 0;

			// Play the audio for the footstep based on footstep boolean.
			if (alternate_footsteps_ == false)
			{
				audio_manager_->PlaySample(6);
			}
			else
			{
				audio_manager_->PlaySample(7);
			}

			// Invert the footstep boolean.
			alternate_footsteps_ = !alternate_footsteps_;
		}
	}
	
	// Update box2d simulation.
	UpdateSimulation(frame_time);

	// Update player.
	player_.Update(frame_time);

	// Update each enemy.
	for (int i = 0; i < enemy_count_; i++)
	{
		enemies_[i].Update(frame_time);
	}

	// Update each crate.
	for (int i = 0; i < crate_count_; i++)
	{
		crates_[i].Update(frame_time);
	}
	
	// Update each sawblade.
	for (int i = 0; i < sawblade_count_; i++)
	{
		sawblades_[i].Update(frame_time);
	}
	
	// Update each crusher.
	for (int i = 0; i < crusher_count_; i++)
	{
		crushers_[i].Update(frame_time);
	}
	
	// Set music to play if it isn't already playing.
	if (music_playing_ == false)
	{
		music_playing_ = true;
		audio_manager_->PlayMusic();
	}

	// Set the volume.
	audio_manager_->SetMasterVolume(*volume_);
}

void Level::Render()
{
	// Setup camera.

	// Projection.
	float fov = gef::DegToRad(45.0f);
	float aspect_ratio = (float)platform_->width() / (float)platform_->height();
	gef::Matrix44 projection_matrix;
	projection_matrix = platform_->PerspectiveProjectionFov(fov, aspect_ratio, 0.1f, 100.0f);
	renderer_3d_->set_projection_matrix(projection_matrix);

	// View.
	gef::Vector4 camera_eye;
	gef::Vector4 camera_lookat;

	// If the player has fallen, the camera will have a fixed y position.
	if (player_.GetBody()->GetPosition().y < 3)
	{
		camera_eye = gef::Vector4(player_.GetBody()->GetPosition().x, 5.0f, 7.5f);
		camera_lookat = gef::Vector4(player_.GetBody()->GetPosition().x, 4.0f, 0.0f);
	}
	else // Otherwise, the camera will follow the player's position.
	{
		camera_eye = gef::Vector4(player_.GetBody()->GetPosition().x, player_.GetBody()->GetPosition().y + 2.0f, 7.5f);
		camera_lookat = gef::Vector4(player_.GetBody()->GetPosition().x, player_.GetBody()->GetPosition().y + 1.0f, 0.0f);
	}

	// Set 3d renderer to use the camera's view matrix.
	gef::Vector4 camera_up(0.0f, 1.0f, 0.0f);
	gef::Matrix44 view_matrix;
	view_matrix.LookAt(camera_eye, camera_lookat, camera_up);
	renderer_3d_->set_view_matrix(view_matrix);


	// Draw 3d geometry.
	renderer_3d_->Begin();

	// Render the ground.

	// Set override material, then render each ground object.
	renderer_3d_->set_override_material(&floor_material_);
	for (int i = 0; i < ground_count_; i++)
	{
		renderer_3d_->DrawMesh(ground_[i]);
	}
	
	// Set override material, then render each wall object.
	renderer_3d_->set_override_material(&wall_material_);
	for (int i = 0; i < wall_count_; i++)
	{
		renderer_3d_->DrawMesh(wall_[i]);
	}
	

	// Render the player.
	renderer_3d_->set_override_material(NULL);
	player_.Render(renderer_3d_);

	// Render the enemies.
	for (int i = 0; i < enemy_count_; i++)
	{
		enemies_[i].Render(renderer_3d_);
	}

	// Set override material and render the crushers.
	renderer_3d_->set_override_material(&metal_material_);
	for (int i = 0; i < crusher_count_; i++)
	{
		renderer_3d_->DrawMesh(crushers_[i]);
	}
	
	// Rendering crates.
	// Determine override material based on type.
	for (int i = 0; i < crate_count_; i++)
	{
		switch (crates_[i].GetType())
		{
		case CrateType::WOOD:
			renderer_3d_->set_override_material(&crate__material_);
			break;
		case CrateType::METAL:
			renderer_3d_->set_override_material(&metal_crate_material_);
			break;
		case CrateType::JUMP_WOOD:
			renderer_3d_->set_override_material(&jump_crate_material_);
			break;
		case CrateType::JUMP_METAL:
			renderer_3d_->set_override_material(&metal_jump_crate_material_);
			break;
		case CrateType::DESTROYED:
			renderer_3d_->set_override_material(&wood_material_);
			crates_[i].RenderPlanks(renderer_3d_); // Render destroyed crate planks with wood material.
			break;
		}

		// Render if not destroyed.
		if (crates_[i].GetType() != CrateType::DESTROYED)
		{
			renderer_3d_->DrawMesh(crates_[i]);
		}	
	}

	// Render the coins contained in each crate once they have been destroyed.
	for (int i = 0; i < crate_count_; i++)
	{
		if (crates_[i].GetType() == CrateType::DESTROYED) 
		{
			renderer_3d_->set_override_material(&coin_material_);
			crates_[i].RenderCoins(renderer_3d_);
		}
	}

	// Set override material and render all of the coins.
	renderer_3d_->set_override_material(&coin_material_);
	for (int i = 0; i < coin_count_; i++)
	{
		if (!coins_[i].GetCollected())
		{
			renderer_3d_->DrawMesh(coins_[i]);
		}
	}

	// Set override material and render all of the checkpoints.
	renderer_3d_->set_override_material(&checkpoint_material_);
	for (int i = 0; i < checkpoint_count_; i++)
	{
		renderer_3d_->DrawMesh(checkpoints_[i]);
	}
	
	// Set override material and render all of the sawblades.
	renderer_3d_->set_override_material(&sawblade_material_);
	for (int i = 0; i < sawblade_count_; i++)
	{
		renderer_3d_->DrawMesh(sawblades_[i]);
	}

	// Finish rendering 3d objects.
	renderer_3d_->End();

	// Start drawing sprites, but don't clear the frame buffer.
	sprite_renderer_->Begin(false);

	// Render the hud, will appear over the 3d objects.
	RenderHud();

	sprite_renderer_->End();
}

void Level::Init(gef::SpriteRenderer* sr, gef::Font* f, gef::Platform* p, GameState* gs, gef::InputManager* im, gef::AudioManager* am, MainMenu* mm, gef::Renderer3D* r3d, PrimitiveBuilder* pb)
{
	// Set values for all of the pointers.
	sprite_renderer_ = sr;
	font_ = f;
	platform_ = p;
	game_state_ = gs;
	input_manager_ = im;
	audio_manager_ = am;
	main_menu_ = mm;
	renderer_3d_ = r3d;
	primitive_builder_ = pb;
	volume_ = main_menu_->GetVolume();
	controller_ = main_menu_->GetController();

	// Tnitialise the physics world.
	b2Vec2 gravity(0.0f, -9.81f);
	world_ = new b2World(gravity);

	// Initialise objects
	InitLights();
	InitPlayer();
	InitGround();
	InitEnemies();
	InitTextures();
	InitCrates();
	InitWall();
	InitCoins();
	InitTraps();
	InitCheckpoints();
}

void Level::Reset()
{
	// Reset the level.
	// Reset music.
	audio_manager_->LoadMusic("audio/hootsforce_symphonic.wav", *platform_);
	music_playing_ = false;

	// Reset lives.
	lives_ = main_menu_->GetLives();
	player_.SetLives(lives_);

	// Reset timers.
	timer_ = 0.0f;
	end_timer_ = 0.0f;

	// Reset score.
	score_ = 0;
	
	// Reset spawn position and other player attributes.
	respawn_position_ = b2Vec2(-8.0f, 3.0f);
	player_.GetBody()->SetTransform(respawn_position_, 0);
	player_.SetRespawnPosition(respawn_position_);
	player_.SetState(PlayerState::IDLE);
	player_.GetBody()->SetLinearVelocity(b2Vec2(0, 0));
	player_.GetBody()->GetFixtureList()->SetSensor(false);

	// Reset checkpoints.
	for (int i = 0; i < checkpoint_count_; i++)
	{
		checkpoints_[i].SetTriggered(false);
	}

	// Reset enemies.
	for (int i = 0; i < enemy_count_; i++)
	{
		enemies_[i].Reset();
	}

	// Reset crates.
	for (int i = 0; i < crate_count_; i++)
	{
		crates_[i].Reset();
	}

	// Reset coins.
	for (int i = 0; i < coin_count_; i++)
	{
		coins_[i].SetCollected(false);
	}
}

void Level::ProcessTouchInput()
{
	// Get touch input.
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

					// Set player to attack when a new touch is detected and the player isn't already kicking, dead or dancing.
					if (player_.GetState() != PlayerState::KICKING && player_.GetState() != PlayerState::DEAD && player_.GetState() != PlayerState::DANCING)
					{
						audio_manager_->PlaySample(1); // Play kick sound.
						player_.Attack();
					}
				}
			}
			else if (active_touch_id_ == touch->id)
			{
				// we are processing touch data with a matching id to the one we are looking for
				if (touch->type == gef::TT_ACTIVE)
				{
					// update an active touch here
				}
				else if (touch->type == gef::TT_RELEASED)
				{
					// the touch we are tracking has been released
					// perform any actions that need to happen when a touch is released here
					// we're not doing anything here apart from resetting the active touch id
					active_touch_id_ = -1;
				}
			}
		}
	}
}

void Level::ProcessKeyboardInput(float frame_time)
{
	// Get keyboad input
	gef::Keyboard* keyboard = input_manager_->keyboard();
	
	// If the escape key is pressed, pause the game.
	if (keyboard->IsKeyPressed(gef::Keyboard::KC_ESCAPE))
	{
		game_state_->SetGameState(State::PAUSED);
	}

	// If the player isn't dead or dancing...
	if (player_.GetState() != PlayerState::DEAD && player_.GetState() != PlayerState::DANCING)
	{
		if (keyboard->IsKeyDown(gef::Keyboard::KC_A)) // Move left if A is pressed.
		{
			player_.MoveLeft(frame_time);
		}
		else if (keyboard->IsKeyDown(gef::Keyboard::KC_D)) // Move right if D is pressed.
		{
			player_.MoveRight(frame_time);
		}
		else if (player_.GetState() == PlayerState::RUNNING) // If player is running but no longer receiving input, they will return to idle.
		{
			player_.SetState(PlayerState::IDLE);
		}

		// If the space key is pressed...
		if (keyboard->IsKeyPressed(gef::Keyboard::KC_SPACE))
		{
			// Jump if the player isn't already jumping, falling or kicking.
			if (player_.GetState() != PlayerState::JUMPING && player_.GetState() != PlayerState::FALLING && player_.GetState() != PlayerState::KICKING)
			{
				player_.Jump();
			}
		}

		// Attack if the F key is pressed and the player isn't already kicking.
		if (keyboard->IsKeyPressed(gef::Keyboard::KC_F))
		{
			if (player_.GetState() != PlayerState::KICKING)
			{
				audio_manager_->PlaySample(1); // Play kick sound.
				player_.Attack();
			}
		}
	}
}

void Level::ProcessControllerInput(float frame_time)
{
	if (*controller_ != 0) // If controller isn't set to none...
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

				// If the player isn't dead or dancing...
				if (player_.GetState() != PlayerState::DEAD && player_.GetState() != PlayerState::DANCING)
				{
					// Move left when stick is moved left or left d pad is down.
					if (controller->buttons_down() & gef_SONY_CTRL_LEFT || left_x_ < -0.66)
					{
						player_.MoveLeft(frame_time);
					}
					else if (controller->buttons_down() & gef_SONY_CTRL_RIGHT || left_x_ > 0.66) // Move right when stick is moved right or right d pad is down.
					{
						player_.MoveRight(frame_time);
					}

					// If A is pressed on Xbox controller or X is pressed on Playstation controller...
					if ((controller->buttons_pressed() & gef_SONY_CTRL_SQUARE && *controller_ == 1) || (controller->buttons_pressed() & gef_SONY_CTRL_CROSS && *controller_ == 2))
					{
						// Jump if not already jumpng, falling or attacking.
						if (player_.GetState() != PlayerState::JUMPING && player_.GetState() != PlayerState::FALLING && player_.GetState() != PlayerState::KICKING)
						{
							player_.Jump();
						}
					}

					// If X is pressed on Xbox controller or square is pressed on Playstation controller...
					if ((controller->buttons_pressed() & gef_SONY_CTRL_CIRCLE && *controller_ == 1) || (controller->buttons_pressed() & gef_SONY_CTRL_SQUARE && *controller_ == 2)) // CIRCLE = X on Xbox
					{
						// Attack if the player isn't already kicking.
						if (player_.GetState() != PlayerState::KICKING)
						{
							audio_manager_->PlaySample(1); // Play kick sound.
							player_.Attack();
						}
					}
				}

				// If start button is pressed, pause the game.
				if ((controller->buttons_pressed() & gef_SONY_CTRL_R2 && *controller_ == 1) || (controller->buttons_pressed() & gef_SONY_CTRL_START && *controller_ == 2)) // R2 = Start on XBOX
				{
					game_state_->SetGameState(State::PAUSED);
				}
			}
		}
	}
}

void Level::InitPlayer()
{
	// Setup the mesh for the player. Can be rendered if you want to show hitbox.
	gef::Vector4 hitbox_half_dimensions(0.5f, 0.8f, 0.5f);
	player_.set_mesh(primitive_builder_->CreateBoxMesh(hitbox_half_dimensions));

	player_half_height_ = hitbox_half_dimensions.y();

	// Create a physics body for the player.
	b2BodyDef player_body_def;
	player_body_def.type = b2_dynamicBody;
	player_body_def.position = b2Vec2(-8.0f, 3.0f);

	// Create a connection between the rigid body and GameObject.
	player_body_def.userData.pointer = reinterpret_cast<uintptr_t>(&player_);

	player_.SetBody(player_body_def, world_);

	// Create the shape for the player.
	b2PolygonShape player_shape;
	player_shape.SetAsBox(hitbox_half_dimensions.x(), hitbox_half_dimensions.y());

	// Create the fixture.
	b2FixtureDef player_fixture_def;
	player_fixture_def.shape = &player_shape;
	player_fixture_def.density = 1.0f;

	// Create the fixture on the rigid body.
	player_.GetBody()->CreateFixture(&player_fixture_def);
	player_.GetBody()->SetFixedRotation(true);
	player_.GetBody()->SetSleepingAllowed(false);

	// Update visuals from simulation data.
	player_.UpdateFromSimulation();

	// Set initial values for lives and respawn position.
	player_.SetLives(lives_);
	player_.SetRespawnPosition(respawn_position_);

	// Initialise things inside the player object.
	player_.Init(platform_);
}

void Level::InitEnemies()
{
	gef::Scene* enemy_scene = new gef::Scene();
	enemy_scene->ReadSceneFromFile(*platform_, "enemy/zombie.scn");
	enemy_scene->CreateMaterials(*platform_);

	// Setup the mesh for the enemy. Can be rendered if you want to show hitbox.
	gef::Vector4 hitbox_half_dimensions(0.3f, 0.8f, 0.5f);
	
	// Create a physics body for the enemy.
	b2BodyDef enemy_body_def;
	enemy_body_def.type = b2_dynamicBody;

	// Create the shape for the enemy.
	b2PolygonShape enemy_shape;
	enemy_shape.SetAsBox(hitbox_half_dimensions.x(), hitbox_half_dimensions.y());

	// Create the fixture.
	b2FixtureDef enemy_fixture_def;
	enemy_fixture_def.shape = &enemy_shape;
	enemy_fixture_def.density = 1.0f;

	for (int i = 0; i < enemy_count_; i++)
	{
		// Apply mesh to the enemy.
		enemies_[i].set_mesh(primitive_builder_->CreateBoxMesh(hitbox_half_dimensions));

		// Setup each enemy's position and path.
		switch (i)
		{
		case 0:
			enemy_body_def.position = b2Vec2(36.0f, 14.5f);
			enemies_[i].SetPath(4, 4);
			break;
		case 1:
			enemy_body_def.position = b2Vec2(56.0f, 14.5f);
			enemies_[i].SetPath(3, 4);
			break;
		case 2:
			enemy_body_def.position = b2Vec2(94.0f, 14.5f);
			enemies_[i].SetPath(2, 0);
			break;
		case 3:
			enemy_body_def.position = b2Vec2(142.0f, 14.5f);
			enemies_[i].SetPath(3, 2);
			break;
		case 4:
			enemy_body_def.position = b2Vec2(158.0f, 14.5f);
			enemies_[i].SetPath(2, 1);
			break;
		case 5:
			enemy_body_def.position = b2Vec2(224.0f, 14.5f);
			enemies_[i].SetPath(2, 1);
			break;
		case 6:
			enemy_body_def.position = b2Vec2(230.0f, 14.5f);
			enemies_[i].SetPath(2, 3);
			break;
		default:
			break;
		}
		
		// Create a connection between the rigid body and GameObject.
		enemy_body_def.userData.pointer = reinterpret_cast<uintptr_t>(&enemies_[i]);

		enemies_[i].SetBody(enemy_body_def, world_);

		// Create the fixture on the rigid body.
		enemies_[i].GetBody()->CreateFixture(&enemy_fixture_def); 

		// Set so it can't rotate.
		enemies_[i].GetBody()->SetFixedRotation(true);

		// Update visuals from simulation data.
		enemies_[i].UpdateFromSimulation();

		// Initialise things inside the enemy object.
		enemies_[i].Init(platform_, enemy_scene);
	}
}

void Level::InitGround()
{
	// Ground dimensions.
	gef::Vector4 ground_half_dimensions;

	// Physics body.
	b2BodyDef body_def;
	body_def.type = b2_staticBody;
	
	// The ground's shape.
	b2PolygonShape shape;

	// The fixture.
	b2FixtureDef fixture_def;

	for (int i = 0; i < ground_count_; i++)
	{
		ground_[i].set_type(OBJECT_TYPE::GROUND);

		switch (i)
		{
		case 0:
			ground_half_dimensions = gef::Vector4(10.0f, 2.5f, 0.5f);
			body_def.position = b2Vec2(0.0f, 0.0f);
			break;
		case 1:
			ground_half_dimensions = gef::Vector4(10.0f, 0.5f, 0.5f);
			body_def.position = b2Vec2(25.0f + ground_half_dimensions.x(), 14.0f);
			break;
		case 2:
			ground_half_dimensions = gef::Vector4(5.0f, 0.5f, 0.5f);
			body_def.position = b2Vec2(52.0f + ground_half_dimensions.x(), 14.0f);
			break;
		case 3:
			ground_half_dimensions = gef::Vector4(5.0f, 0.5f, 0.5f);
			body_def.position = b2Vec2(67.0f + ground_half_dimensions.x(), 14.0f);
			break;
		case 4:
			ground_half_dimensions = gef::Vector4(13.0f, 0.5f, 0.5f);
			body_def.position = b2Vec2(90.0f + ground_half_dimensions.x(), 14.0f);
			break;
		case 5:
			ground_half_dimensions = gef::Vector4(4.0f, 0.5f, 0.5f);
			body_def.position = b2Vec2(120.0f + ground_half_dimensions.x(), 14.0f);
			break;
		case 6:
			ground_half_dimensions = gef::Vector4(19.0f, 0.5f, 0.5f);
			body_def.position = b2Vec2(132.0f + ground_half_dimensions.x(), 14.0f);
			break;
		case 7:
			ground_half_dimensions = gef::Vector4(10.0f, 0.5f, 0.5f);
			body_def.position = b2Vec2(192.0f + ground_half_dimensions.x(), 14.0f);
			break;
		case 8:
			ground_half_dimensions = gef::Vector4(7.5f, 0.5f, 0.5f);
			body_def.position = b2Vec2(220.0f + ground_half_dimensions.x(), 14.0f);
			break;
		case 9:
			ground_half_dimensions = gef::Vector4(5.0f, 0.5f, 0.5f);
			body_def.position = b2Vec2(250.0f + ground_half_dimensions.x(), 24.0f);
			break;
		default:
			ground_half_dimensions = gef::Vector4(0.0f, 0.0f, 0.0f);
			body_def.position = b2Vec2(0.0f, 0.0f);
			break;
		}

		// Setup the mesh for the ground.
		gef::Mesh* ground_mesh = primitive_builder_->CreateBoxMesh(ground_half_dimensions);
		ground_[i].set_mesh(ground_mesh);

		// Setup the physics body for the ground.
		body_def.userData.pointer = reinterpret_cast<uintptr_t>(&ground_[i]);
		ground_[i].SetBody(body_def, world_);

		// Setup shape and fixture def.
		shape.SetAsBox(ground_half_dimensions.x(), ground_half_dimensions.y());
		fixture_def.shape = &shape;

		// Create the fixture on the rigid body.
		ground_[i].GetBody()->CreateFixture(&fixture_def);

		// Update visuals from simulation data.
		ground_[i].UpdateFromSimulation();
	}
}

void Level::InitLights()
{
	// grab the data for the default shader used for rendering 3D geometry
	gef::Default3DShaderData& default_shader_data = renderer_3d_->default_shader_data();

	// set the ambient light
	default_shader_data.set_ambient_light_colour(gef::Colour(0.25f, 0.25f, 0.25f, 1.0f));

	// add a point light that is almost white, but with a blue tinge
	// the position of the light is set far away so it acts light a directional light
	gef::PointLight default_point_light;
	default_point_light.set_colour(gef::Colour(0.7f, 0.7f, 1.0f, 1.0f));
	default_point_light.set_position(gef::Vector4(-500.0f, 400.0f, 700.0f));
	default_shader_data.AddPointLight(default_point_light);
}

void Level::InitTextures()
{
	// Load each texture from a file, then apply it to the relevant material.
	gef::ImageData texture_image;
	gef::PNGLoader png_loader;
	gef::Texture* texture;

	png_loader.Load("textures/floor.png", *platform_, texture_image);
	texture = gef::Texture::Create(*platform_, texture_image);
	floor_material_.set_texture(texture);

	png_loader.Load("textures/crate.png", *platform_, texture_image);
	texture = gef::Texture::Create(*platform_, texture_image);
	crate__material_.set_texture(texture);

	png_loader.Load("textures/jump_crate.png", *platform_, texture_image);
	texture = gef::Texture::Create(*platform_, texture_image);
	jump_crate_material_.set_texture(texture);

	png_loader.Load("textures/metal_crate.png", *platform_, texture_image);
	texture = gef::Texture::Create(*platform_, texture_image);
	metal_crate_material_.set_texture(texture);

	png_loader.Load("textures/jump_metal_crate.png", *platform_, texture_image);
	texture = gef::Texture::Create(*platform_, texture_image);
	metal_jump_crate_material_.set_texture(texture);

	png_loader.Load("textures/metal.png", *platform_, texture_image);
	texture = gef::Texture::Create(*platform_, texture_image);
	metal_material_.set_texture(texture);

	png_loader.Load("textures/wall.png", *platform_, texture_image);
	texture = gef::Texture::Create(*platform_, texture_image);
	wall_material_.set_texture(texture);

	png_loader.Load("textures/wood.png", *platform_, texture_image);
	texture = gef::Texture::Create(*platform_, texture_image);
	wood_material_.set_texture(texture);

	png_loader.Load("textures/coin.png", *platform_, texture_image);
	texture = gef::Texture::Create(*platform_, texture_image);
	coin_material_.set_texture(texture);

	png_loader.Load("textures/sawblade.png", *platform_, texture_image);
	texture = gef::Texture::Create(*platform_, texture_image);
	sawblade_material_.set_texture(texture);

	png_loader.Load("textures/checkpoint.png", *platform_, texture_image);
	texture = gef::Texture::Create(*platform_, texture_image);
	checkpoint_material_.set_texture(texture);
}

void Level::InitCrates()
{
	// Setup the mesh for the crate.
	gef::Vector4 hitbox_half_dimensions(0.5f, 0.5f, 0.5f);

	crate_half_height_ = hitbox_half_dimensions.y();

	// Create a physics body for the crate.
	b2BodyDef crate_body_def;
	crate_body_def.type = b2_staticBody;

	// Create the shape for the crate.
	b2PolygonShape crate_shape;
	crate_shape.SetAsBox(hitbox_half_dimensions.x(), hitbox_half_dimensions.y());

	// Create the fixture.
	b2FixtureDef crate_fixture_def;
	crate_fixture_def.shape = &crate_shape;
	crate_fixture_def.density = 1.0f;

	for (int i = 0; i < crate_count_; i++)
	{
		// Set crate's object type.
		crates_[i].set_type(OBJECT_TYPE::CRATE);

		// Create crate's mesh.
		crates_[i].set_mesh(primitive_builder_->CreateBoxMesh(hitbox_half_dimensions));
	

		
		// Set each crates position and type.
		switch (i)
		{
		case 0:
			crate_body_def.position = b2Vec2(0.0f, 3.0f);
			crates_[i].SetType(CrateType::WOOD);
			break;
		case 1:
			crate_body_def.position = b2Vec2(5.0f, 3.0f);
			crates_[i].SetType(CrateType::JUMP_WOOD);
			break;
		case 2:
			crate_body_def.position = b2Vec2(5.0f, 7.0f);
			crates_[i].SetType(CrateType::WOOD);
			break;
		case 3:
			crate_body_def.position = b2Vec2(9.5f, 3.0f);
			crates_[i].SetType(CrateType::JUMP_METAL);
			break;
		case 4:
			crate_body_def.position = b2Vec2(15.0f, 7.0f);
			crates_[i].SetType(CrateType::JUMP_METAL);
			break;
		case 5:
			crate_body_def.position = b2Vec2(20.0f, 11.0f);
			crates_[i].SetType(CrateType::JUMP_METAL);
			break;
		case 6:
			crate_body_def.position = b2Vec2(45.5f, 14.0f);
			crates_[i].SetType(CrateType::WOOD);
			break;
		case 7:
			crate_body_def.position = b2Vec2(46.5f, 14.0f);
			crates_[i].SetType(CrateType::WOOD);
			break;
		case 8:
			crate_body_def.position = b2Vec2(47.5f, 14.0f);
			crates_[i].SetType(CrateType::WOOD);
			break;
		case 9:
			crate_body_def.position = b2Vec2(48.5f, 14.0f);
			crates_[i].SetType(CrateType::METAL);
			break;
		case 10:
			crate_body_def.position = b2Vec2(49.5f, 14.0f);
			crates_[i].SetType(CrateType::WOOD);
			break;
		case 11:
			crate_body_def.position = b2Vec2(50.5f, 14.0f);
			crates_[i].SetType(CrateType::WOOD);
			break;
		case 12:
			crate_body_def.position = b2Vec2(51.5f, 14.0f);
			crates_[i].SetType(CrateType::WOOD);
			break;
		case 13:
			crate_body_def.position = b2Vec2(70.0f, 15.0f);
			crates_[i].SetType(CrateType::WOOD);
			break;
		case 14:
			crate_body_def.position = b2Vec2(71.5f, 16.0f);
			crates_[i].SetType(CrateType::WOOD);
			break;
		case 15:
			crate_body_def.position = b2Vec2(73.5f, 19.0f);
			crates_[i].SetType(CrateType::WOOD);
			break;
		case 16:
			crate_body_def.position = b2Vec2(81.0f, 14.0f);
			crates_[i].SetType(CrateType::METAL);
			break;
		case 17:
			crate_body_def.position = b2Vec2(86.0f, 14.0f);
			crates_[i].SetType(CrateType::METAL);
			break;
		case 18:
			crate_body_def.position = b2Vec2(112.0f, 15.0f);
			crates_[i].SetType(CrateType::WOOD);
			break;
		case 19:
			crate_body_def.position = b2Vec2(135.0f, 15.0f);
			crates_[i].SetType(CrateType::JUMP_WOOD);
			break;
		case 20:
			crate_body_def.position = b2Vec2(133.0f, 19.0f);
			crates_[i].SetType(CrateType::WOOD);
			break;
		case 21:
			crate_body_def.position = b2Vec2(137.0f, 19.0f);
			crates_[i].SetType(CrateType::WOOD);
			break;
		case 22:
			crate_body_def.position = b2Vec2(168.0f, 20.0f);
			crates_[i].SetType(CrateType::WOOD);
			break;
		case 23:
			crate_body_def.position = b2Vec2(172.0f, 14.0f);
			crates_[i].SetType(CrateType::METAL);
			break;
		case 24:
			crate_body_def.position = b2Vec2(175.0f, 16.0f);
			crates_[i].SetType(CrateType::METAL);
			break;
		case 25:
			crate_body_def.position = b2Vec2(172.0f, 18.0f);
			crates_[i].SetType(CrateType::METAL);
			break;
		case 26:
			crate_body_def.position = b2Vec2(176.0f, 20.0f);
			crates_[i].SetType(CrateType::METAL);
			break;
		case 27:
			crate_body_def.position = b2Vec2(182.0f, 22.0f);
			crates_[i].SetType(CrateType::WOOD);
			break;
		case 28:
			crate_body_def.position = b2Vec2(180.0f, 14.0f);
			crates_[i].SetType(CrateType::JUMP_METAL);
			break;
		case 29:
			crate_body_def.position = b2Vec2(181.0f, 14.0f);
			crates_[i].SetType(CrateType::JUMP_METAL);
			break;
		case 30:
			crate_body_def.position = b2Vec2(182.0f, 14.0f);
			crates_[i].SetType(CrateType::JUMP_METAL);
			break;
		case 31:
			crate_body_def.position = b2Vec2(187.0f, 18.0f);
			crates_[i].SetType(CrateType::METAL);
			break;
		case 32:
			crate_body_def.position = b2Vec2(209.0f, 15.0f);
			crates_[i].SetType(CrateType::JUMP_WOOD);
			break;
		case 33:
			crate_body_def.position = b2Vec2(205.5f, 20.0f);
			crates_[i].SetType(CrateType::WOOD);
			break;
		case 34:
			crate_body_def.position = b2Vec2(200.5f, 20.0f);
			crates_[i].SetType(CrateType::WOOD);
			break;
		case 35:
			crate_body_def.position = b2Vec2(216.0f, 14.0f);
			crates_[i].SetType(CrateType::METAL);
			break;
		case 36:
			crate_body_def.position = b2Vec2(240.0f, 14.0f);
			crates_[i].SetType(CrateType::JUMP_METAL);
			break;
		case 37:
			crate_body_def.position = b2Vec2(245.0f, 19.0f);
			crates_[i].SetType(CrateType::JUMP_METAL);
			break;
		case 38:
			crate_body_def.position = b2Vec2(235.0f, 19.0f);
			crates_[i].SetType(CrateType::WOOD);
			break;
		case 39:
			crate_body_def.position = b2Vec2(240.0f, 24.0f);
			crates_[i].SetType(CrateType::WOOD);
			break;
		default:
			break;
		}
		
		// Create a connection between the rigid body and GameObject.
		crate_body_def.userData.pointer = reinterpret_cast<uintptr_t>(&crates_[i]);

		crates_[i].SetBody(crate_body_def, world_);

		// Create the fixture on the rigid body.
		crates_[i].GetBody()->CreateFixture(&crate_fixture_def);

		// Set so crate can't rotate.
		crates_[i].GetBody()->SetFixedRotation(true);

		// Update visuals from simulation data.
		crates_[i].UpdateFromSimulation();

		// Initialise things inside the crate.
		crates_[i].Init(primitive_builder_, world_);
	}
}

void Level::InitWall()
{
	// Wall dimensions.
	gef::Vector4 wall_half_dimensions(10.0f, 10.0f, 0.5f);

	// Setup the mesh for the wall.
	gef::Mesh* wall_mesh = primitive_builder_->CreateBoxMesh(wall_half_dimensions);
	
	// Variables for setting wall transformation.
	gef::Matrix44 rotX, rotY, rotZ, trans, final, scale;

	scale.Scale(gef::Vector4(1, 1, 1));
	rotX.RotationX(0);
	rotY.RotationY(0);
	rotZ.RotationZ(0);
	

	for (int i = 0; i < wall_count_; i++)
	{
		// Apply mesh to wall.
		wall_[i].set_mesh(wall_mesh);

		gef::Vector4 position;
		
		if (i == wall_count_ - 1) // Only one wall needs a physics body for collisions with - this is placed at the start of the level.
		{
			// Set position.
			position = gef::Vector4(-2 * wall_half_dimensions.x(), wall_half_dimensions.y(), 0);

			// Define body def.
			b2BodyDef wall_body_def;
			wall_body_def.type = b2_staticBody;
			wall_body_def.position = b2Vec2(position.x(), position.y());

			// Create a connection between the rigid body and GameObject.
			wall_body_def.userData.pointer = reinterpret_cast<uintptr_t>(&wall_[i]);

			wall_[i].SetBody(wall_body_def, world_);

			// Create the shape for the wall.
			b2PolygonShape wall_shape;
			wall_shape.SetAsBox(wall_half_dimensions.x(), wall_half_dimensions.y());

			// Create the fixture.
			b2FixtureDef wall_fixture_def;
			wall_fixture_def.shape = &wall_shape;
			wall_fixture_def.density = 1.0f;

			// Create the fixture on the rigid body.
			wall_[i].GetBody()->CreateFixture(&wall_fixture_def);
			wall_[i].GetBody()->SetFixedRotation(true);
		}
		// Next if and else position the walls in 2 rows.
		else if (i < (wall_count_ / 2) - 1)
		{
			position = gef::Vector4(i * 2 * wall_half_dimensions.x(), wall_half_dimensions.y(), -1);
		}
		else
		{
			position = gef::Vector4((i - (wall_count_ / 2) - 1) * 2 * wall_half_dimensions.x(), 3 * wall_half_dimensions.y(), -1);
		}
		trans.SetIdentity();
		trans.SetTranslation(position);

		final = scale * rotX * rotY * rotZ * trans;

		// Apply transformation to the walls.
		wall_[i].set_transform(final);
	}
}

void Level::InitCoins()
{
	// Setup the mesh for the coin.
	gef::Vector4 hitbox_half_dimensions(0.3f, 0.3f, 0.0f);

	// Create a physics body for the coin.
	b2BodyDef coin_body_def;
	coin_body_def.type = b2_staticBody;

	// Create the shape for the coin.
	b2PolygonShape coin_shape;
	coin_shape.SetAsBox(hitbox_half_dimensions.x(), hitbox_half_dimensions.y());

	// Create the fixture.
	b2FixtureDef coin_fixture_def;
	coin_fixture_def.shape = &coin_shape;
	coin_fixture_def.density = 1.0f;

	for (int i = 0; i < coin_count_; i++)
	{
		// Set coin's object type.
		coins_[i].set_type(OBJECT_TYPE::COIN);
		
		// Apply mesh to the coin.
		coins_[i].set_mesh(primitive_builder_->CreateBoxMesh(hitbox_half_dimensions));
	
		// Position each coin.
		switch (i)
		{
		case 0:
			coin_body_def.position = b2Vec2(-6.0f, 3.5f);
			break;
		case 1:
			coin_body_def.position = b2Vec2(-4.0f, 3.5f);
			break;
		case 2:
			coin_body_def.position = b2Vec2(-2.0f, 3.5f);
			break;
		case 3:
			coin_body_def.position = b2Vec2(9.5f, 4.5f);
			break;
		case 4:
			coin_body_def.position = b2Vec2(9.5f, 6.5f);
			break;
		case 5:
			coin_body_def.position = b2Vec2(9.5f, 8.5f);
			break;
		case 6:
			coin_body_def.position = b2Vec2(15.0f, 8.5f);
			break;
		case 7:
			coin_body_def.position = b2Vec2(15.0f, 10.5f);
			break;
		case 8:
			coin_body_def.position = b2Vec2(15.0f, 12.5f);
			break;
		case 9:
			coin_body_def.position = b2Vec2(20.0f, 12.5f);
			break;
		case 10:
			coin_body_def.position = b2Vec2(20.0f, 14.5f);
			break;
		case 11:
			coin_body_def.position = b2Vec2(20.0f, 16.5f);
			break;
		case 12:
			coin_body_def.position = b2Vec2(81.0f, 15.5f);
			break;
		case 13:
			coin_body_def.position = b2Vec2(86.0f, 15.5f);
			break;
		case 14:
			coin_body_def.position = b2Vec2(120.0f, 16.5f);
			break;
		case 15:
			coin_body_def.position = b2Vec2(124.0f, 16.5f);
			break;
		case 16:
			coin_body_def.position = b2Vec2(128.0f, 16.5f);
			break;
		case 17:
			coin_body_def.position = b2Vec2(135.0f, 16.5f);
			break;
		case 18:
			coin_body_def.position = b2Vec2(135.0f, 18.5f);
			break;
		case 19:
			coin_body_def.position = b2Vec2(135.0f, 20.5f);
			break;
		case 20:
			coin_body_def.position = b2Vec2(148.0f, 15.5f);
			break;
		case 21:
			coin_body_def.position = b2Vec2(151.0f, 15.5f);
			break;
		case 22:
			coin_body_def.position = b2Vec2(154.0f, 15.5f);
			break;
		case 23:
			coin_body_def.position = b2Vec2(172.0f, 15.5f);
			break;
		case 24:
			coin_body_def.position = b2Vec2(175.0f, 17.5f);
			break;
		case 25:
			coin_body_def.position = b2Vec2(172.0f, 19.5f);
			break;
		case 26:
			coin_body_def.position = b2Vec2(176.0f, 21.5f);
			break;
		case 27:
			coin_body_def.position = b2Vec2(187.0f, 19.5f);
			break;
		case 28:
			coin_body_def.position = b2Vec2(209.0f, 16.5f);
			break;
		case 29:
			coin_body_def.position = b2Vec2(209.0f, 18.5f);
			break;
		case 30:
			coin_body_def.position = b2Vec2(209.0f, 20.5f);
			break;
		case 31:
			coin_body_def.position = b2Vec2(240.0f, 15.5f);
			break;
		case 32:
			coin_body_def.position = b2Vec2(240.0f, 17.5f);
			break;
		case 33:
			coin_body_def.position = b2Vec2(240.0f, 19.5f);
			break;
		case 34:
			coin_body_def.position = b2Vec2(245.0f, 20.5f);
			break;
		case 35:
			coin_body_def.position = b2Vec2(245.0f, 22.5f);
			break;
		case 36:
			coin_body_def.position = b2Vec2(245.0f, 24.5f);
			break;
		default:
			break;
		}


		// Create a connection between the rigid body and GameObject.
		coin_body_def.userData.pointer = reinterpret_cast<uintptr_t>(&coins_[i]);

		coins_[i].SetBody(coin_body_def, world_);

		// Create the fixture on the rigid body.
		coins_[i].GetBody()->CreateFixture(&coin_fixture_def);

		// Set coin to have no rotation and be a sensor.
		coins_[i].GetBody()->SetFixedRotation(true);
		coins_[i].GetBody()->GetFixtureList()->SetSensor(true);

		// Update visuals from simulation data.
		coins_[i].UpdateFromSimulation();
	}
}

void Level::InitTraps()
{
	// Half dimensions of the sawblade.
	gef::Vector4 saw_half_dimensions;

	// Create a physics body for the sawblade.
	b2BodyDef saw_body_def;
	saw_body_def.type = b2_staticBody;
	saw_body_def.position = b2Vec2(0.0f, 0.0f);

	// Shape for the sawblade.
	b2PolygonShape saw_shape;

	// Create the fixture.
	b2FixtureDef saw_fixture_def;
	saw_fixture_def.shape = &saw_shape;
	saw_fixture_def.density = 1.0f;

	for (int i = 0; i < sawblade_count_; i++)
	{
		sawblades_[i].set_type(OBJECT_TYPE::SAWBLADE);
		if (i == 3)
		{
			saw_half_dimensions = gef::Vector4(0.5f, 0.5f, 0.0f);
		}
		else
		{
			saw_half_dimensions = gef::Vector4(1.0f, 1.0f, 0.0f);
		}
		sawblades_[i].set_mesh(primitive_builder_->CreateBoxMesh(saw_half_dimensions));
		
	
		// Create a connection between the rigid body and GameObject.
		saw_body_def.userData.pointer = reinterpret_cast<uintptr_t>(&sawblades_[i]);

		sawblades_[i].SetBody(saw_body_def, world_);

		if (i == 3)
		{
			saw_shape.SetAsBox(0.8 * saw_half_dimensions.x(), 0.8 * saw_half_dimensions.y());
		}
		else
		{
			saw_shape.SetAsBox(0.8 * saw_half_dimensions.x(), 0.8 * saw_half_dimensions.y());
		}

		// Create the fixture on the rigid body.
		sawblades_[i].GetBody()->CreateFixture(&saw_fixture_def);

		// Set rotation to be fixed and object to be a sensor.
		sawblades_[i].GetBody()->SetFixedRotation(true);
		sawblades_[i].GetBody()->GetFixtureList()->SetSensor(true);

		// Position and initialise each sawblade.
		switch (i)
		{
		case 0:
			sawblades_[i].GetBody()->SetTransform(b2Vec2(79, 11), 0);
			sawblades_[i].Init(2, 0, 6);
			break;
		case 1:
			sawblades_[i].GetBody()->SetTransform(b2Vec2(83.5, 11), 0);
			sawblades_[i].Init(4, 0, 6);
			break;
		case 2:
			sawblades_[i].GetBody()->SetTransform(b2Vec2(88, 11), 0);
			sawblades_[i].Init(2, 0, 6);
			break;
		case 3:
			sawblades_[i].GetBody()->SetTransform(b2Vec2(124, 15.5), 0);
			sawblades_[i].Init(0, 2, 3);
			break;
		case 4:
			sawblades_[i].GetBody()->SetTransform(b2Vec2(200.5, 15.5), 0);
			sawblades_[i].Init(2, 0, 3);
			break;
		case 5:
			sawblades_[i].GetBody()->SetTransform(b2Vec2(205.5, 15.5), 0);
			sawblades_[i].Init(2, 0, 3);
			break;
		default:
			break;
		}

		
		// Update visuals from simulation data.
		sawblades_[i].UpdateFromSimulation();
	}
	
	// The crusher's half dimensions.
	gef::Vector4 crusher_half_dimensions(1.0f, 2.0f, 0.5f);

	crusher_half_height_ = crusher_half_dimensions.y();

	// Create a physics body for the crusher.
	b2BodyDef crusher_body_def;
	crusher_body_def.type = b2_staticBody;
	crusher_body_def.position = b2Vec2(0.0f, 0.0f);

	// Create the shape for the crusher.
	b2PolygonShape crusher_shape;
	crusher_shape.SetAsBox(crusher_half_dimensions.x(), crusher_half_dimensions.y());

	// Create the fixture.
	b2FixtureDef crusher_fixture_def;
	crusher_fixture_def.shape = &crusher_shape;
	crusher_fixture_def.density = 1.0f;

	for (int i = 0; i < crusher_count_; i++)
	{
		// Set game object type to crusher.
		crushers_[i].set_type(OBJECT_TYPE::CRUSHER);

		// Create mesh for crusher.
		crushers_[i].set_mesh(primitive_builder_->CreateBoxMesh(crusher_half_dimensions));
		
		// Create a connection between the rigid body and GameObject.
		crusher_body_def.userData.pointer = reinterpret_cast<uintptr_t>(&crushers_[i]);

		crushers_[i].SetBody(crusher_body_def, world_);


		// Create the fixture on the rigid body.
		crushers_[i].GetBody()->CreateFixture(&crusher_fixture_def);
		crushers_[i].GetBody()->SetFixedRotation(true);

		// Position and initialise each crusher.
		switch (i)
		{
		case 0:
			crushers_[i].GetBody()->SetTransform(b2Vec2(100, 20), 0);
			crushers_[i].Init(0, 3);
			break;
		case 1:
			crushers_[i].GetBody()->SetTransform(b2Vec2(104, 20), 0);
			crushers_[i].Init(3, 3);
			break;
		case 2:
			crushers_[i].GetBody()->SetTransform(b2Vec2(108, 20), 0);
			crushers_[i].Init(0, 3);
			break;
		case 3:
			crushers_[i].GetBody()->SetTransform(b2Vec2(148, 20), 0);
			crushers_[i].Init(0, 0.5);
			break;
		case 4:
			crushers_[i].GetBody()->SetTransform(b2Vec2(151, 20), 0);
			crushers_[i].Init(0.5, 0.5);
			break;
		case 5:
			crushers_[i].GetBody()->SetTransform(b2Vec2(154, 20), 0);
			crushers_[i].Init(1, 0.5);
			break;
		case 6:
			crushers_[i].GetBody()->SetTransform(b2Vec2(203, 20), 0);
			crushers_[i].Init(0, 3);
			break;
		default:
			break;
		}

		// Update visuals from simulation data.
		crushers_[i].UpdateFromSimulation();
	}
	
}

void Level::InitCheckpoints()
{
	// Checkpoint's half dimensions.
	gef::Vector4 hitbox_half_dimensions(0.3f, 0.3f, 0.0f);

	// Create a physics body for the checkpoint.
	b2BodyDef checkpoint_body_def;
	checkpoint_body_def.type = b2_staticBody;

	// Create the shape for the checkpoint.
	b2PolygonShape checkpoint_shape;
	checkpoint_shape.SetAsBox(hitbox_half_dimensions.x(), hitbox_half_dimensions.y() + 1); // Add to the hitbox's y dimension to avoid player jumping over checkpoint and missing it.

	// create the fixture
	b2FixtureDef checkpoint_fixture_def;
	checkpoint_fixture_def.shape = &checkpoint_shape;
	checkpoint_fixture_def.density = 1.0f;

	for (int i = 0; i < checkpoint_count_; i++)
	{
		// Set game object's type to checkpoint.
		checkpoints_[i].set_type(OBJECT_TYPE::CHECKPOINT);

		// Setup the mesh for the checkpoint.
		checkpoints_[i].set_mesh(primitive_builder_->CreateBoxMesh(hitbox_half_dimensions));
		
		// Position each checkpoint.
		switch (i)
		{
		case 0:
			checkpoint_body_def.position = b2Vec2(68.0f, 15.0f);
			break;
		case 1:
			checkpoint_body_def.position = b2Vec2(114.0f, 15.0f);
			break;
		case 2:
			checkpoint_body_def.position = b2Vec2(168.0f, 15.0f);
			break;
		case 3:
			checkpoint_body_def.position = b2Vec2(253.0f, 25.0f);
			break;
		default:
			break;
		}

		// Set the checkpoint's transform.
		gef::Matrix44 rotX, rotY, rotZ, trans, final, scale;

		scale.Scale(gef::Vector4(1, 1, 1));
		rotX.RotationX(0);
		rotY.RotationY(0);
		rotZ.RotationZ(0);
		gef::Vector4 position = gef::Vector4(checkpoint_body_def.position.x, checkpoint_body_def.position.y, -0.49);
		trans.SetIdentity();
		trans.SetTranslation(position);

		final = scale * rotX * rotY * rotZ * trans;

		checkpoints_[i].set_transform(final);

		// Create a connection between the rigid body and GameObject.
		checkpoint_body_def.userData.pointer = reinterpret_cast<uintptr_t>(&checkpoints_[i]);

		checkpoints_[i].SetBody(checkpoint_body_def, world_);

		// Create the fixture on the rigid body.
		checkpoints_[i].GetBody()->CreateFixture(&checkpoint_fixture_def);

		// Set to have fixed rotation and be a sensor.
		checkpoints_[i].GetBody()->SetFixedRotation(true);
		checkpoints_[i].GetBody()->GetFixtureList()->SetSensor(true);
	}
}

void Level::UpdateSimulation(float frame_time)
{
	// Update physics world.
	float timeStep = 1.0f / 60.0f;

	int32 velocityIterations = 6;
	int32 positionIterations = 2;

	world_->Step(timeStep, velocityIterations, positionIterations);

	// Update player visuals from simulation data.
	player_.UpdateFromSimulation();

	// Update enemy visuals.
	for (int i = 0; i < enemy_count_; i++)
	{
		enemies_[i].UpdateFromSimulation();
	}

	// Update crate's coins and planks' visuals if it's destroyed.
	for (int i = 0; i < crate_count_; i++)
	{
		if (crates_[i].GetType() == CrateType::DESTROYED)
		{
			crates_[i].UpdateDestroyedSimulation();
		}
	}

	// Update sawblades' visuals.
	for (int i = 0; i < sawblade_count_; i++)
	{
		sawblades_[i].UpdateFromSimulation();
	}
	
	// Update crushers' visuals.
	for (int i = 0; i < crusher_count_; i++)
	{
		crushers_[i].UpdateFromSimulation();
	}


	// Collision detection.
	// Get the head of the contact list.
	b2Contact* contact = world_->GetContactList();
	// Get contact count.
	int contact_count = world_->GetContactCount();

	for (int contact_num = 0; contact_num < contact_count; ++contact_num)
	{
		if (contact->IsTouching())
		{
			// get the colliding bodies
			b2Body* bodyA = contact->GetFixtureA()->GetBody();
			b2Body* bodyB = contact->GetFixtureB()->GetBody();

			// Pointers for the different kinds of objects that could collide.
			Player* player = NULL;
			Enemy* enemy = NULL;
			Crate* crate = NULL;
			GameObject* ground = NULL;
			Coin* coin = NULL;
			Sawblade* sawblade = NULL;
			Crusher* crusher = NULL;
			Checkpoint* checkpoint = NULL;

			GameObject* gameObjectA = NULL;
			GameObject* gameObjectB = NULL;

			gameObjectA = reinterpret_cast<GameObject*>(bodyA->GetUserData().pointer);
			gameObjectB = reinterpret_cast<GameObject*>(bodyB->GetUserData().pointer);

			// Cast the first game object based on their type.
			if (gameObjectA)
			{
				if (gameObjectA->type() == PLAYER)
				{
					player = reinterpret_cast<Player*>(bodyA->GetUserData().pointer);
				}
				else if(gameObjectA->type() == ENEMY)
				{
					enemy = reinterpret_cast<Enemy*>(bodyA->GetUserData().pointer);
				}
				else if (gameObjectA->type() == CRATE)
				{
					crate = reinterpret_cast<Crate*>(bodyA->GetUserData().pointer);
				}
				else if (gameObjectA->type() == GROUND)
				{
					ground = gameObjectA;
				}
				else if (gameObjectA->type() == COIN)
				{
					coin = reinterpret_cast<Coin*>(bodyA->GetUserData().pointer);
				}
				else if (gameObjectA->type() == SAWBLADE)
				{
					sawblade = reinterpret_cast<Sawblade*>(bodyA->GetUserData().pointer);
				}
				else if (gameObjectA->type() == CRUSHER)
				{
					crusher = reinterpret_cast<Crusher*>(bodyA->GetUserData().pointer);
				}
				else if (gameObjectA->type() == CHECKPOINT)
				{
					checkpoint = reinterpret_cast<Checkpoint*>(bodyA->GetUserData().pointer);
				}
			}

			// Cast the second game object based on their type.
			if (gameObjectB)
			{
				if (gameObjectB->type() == PLAYER)
				{
					player = reinterpret_cast<Player*>(bodyB->GetUserData().pointer);
				}
				else if (gameObjectB->type() == ENEMY)
				{
					enemy = reinterpret_cast<Enemy*>(bodyB->GetUserData().pointer);
				}
				else if (gameObjectB->type() == CRATE)
				{
					crate = reinterpret_cast<Crate*>(bodyB->GetUserData().pointer);
				}
				else if (gameObjectB->type() == GROUND)
				{
					ground = gameObjectB;
				}
				else if (gameObjectB->type() == COIN)
				{
					coin = reinterpret_cast<Coin*>(bodyB->GetUserData().pointer);
				}
				else if (gameObjectB->type() == SAWBLADE)
				{
					sawblade = reinterpret_cast<Sawblade*>(bodyB->GetUserData().pointer);
				}
				else if (gameObjectB->type() == CRUSHER)
				{
					crusher = reinterpret_cast<Crusher*>(bodyB->GetUserData().pointer);
				}
				else if (gameObjectB->type() == CHECKPOINT)
				{
					checkpoint = reinterpret_cast<Checkpoint*>(bodyB->GetUserData().pointer);
				}
			}

			// If one of the objects is a crusher, and it's currently crushing...
			if (crusher && crusher->GetCrushing())
			{
				// If the second object is the ground...
				if (ground)
				{
					// Play metallic clang sound if player is close enough.
					if (player_.GetBody()->GetPosition().x > crusher->GetBody()->GetPosition().x - audio_proximity_ && player_.GetBody()->GetPosition().x < crusher->GetBody()->GetPosition().x + audio_proximity_)
					{
						audio_manager_->PlaySample(9);
					}

					// Set the crusher to be finished.
					crusher->SetFinished(true);
				}
			}

			// If the player is involved with the collision...
			if (player)
			{
				// If the player lands on the ground, an enemy or a crate.
				if (player->GetState() == PlayerState::FALLING && (ground || enemy || crate))
				{
					if (crate && crate->GetType() == CrateType::DESTROYED)
					{
						// Don't change state if it's a destroyed crate. Prevents players from landing on broken/invisible crates.
					}
					else
					{
						// Set player's state to landing.
						player->SetState(PlayerState::LANDING);
					}
					
				}

				// If the player collides with an untriggered checkpoint, trigger the checkpoint.
				if (checkpoint && !checkpoint->GetTriggered())
				{
					checkpoint->SetTriggered(true);
				}

				// If the player collides with an enemy and isn't already dead...
				if (enemy && player->GetState() != PlayerState::DEAD)
				{
					// If the enemy isn't dead...
					if (enemy->GetState() != EnemyState::DEAD)
					{
						// Calculate the rough direction that the player is relative to the enemy (the difference between the position of the objects).
						float difX, difY;
						difX = player->GetBody()->GetPosition().x - enemy->GetBody()->GetPosition().x;
						difY = player->GetBody()->GetPosition().y - enemy->GetBody()->GetPosition().y;

						// If the player's kicking...
						if (player->GetState() == PlayerState::KICKING)
						{
							// If difX < 0, it's roughly left.
							if (difX < 0)
							{
								enemy->SetDead(Direction::LEFT); // Launch enemy based on the attacking direction.
								audio_manager_->PlaySample(4); // Play enemy death sound.
							}
							else // Otherwise it'll be right.
							{
								enemy->SetDead(Direction::RIGHT); // Launch enemy based on the attacking direction.
								audio_manager_->PlaySample(4); // Play enemy death sound.
							}
						}
						else if (difY > 0) // If the player lands around the enemy's head...
						{
							enemy->SetDead(Direction::UP); // Launch enemy based on the attacking direction.
							player->GetBody()->ApplyForceToCenter(b2Vec2(0, 400), true); // Launch player up a bit.
							player->SetState(PlayerState::JUMPING); // Set player's state to jumping.
							audio_manager_->PlaySample(2); // Play bounce sound.
						}
						else
						{
							// The enemy kills the player.
							player->SetDead();
							audio_manager_->PlaySample(5); // Play death sound.
						}
					}
				}

				if (crate)
				{
					// For whether the player is above or below the crate
					float difY;
					difY = player->GetBody()->GetPosition().y - player_half_height_ - crate->GetBody()->GetPosition().y - crate_half_height_;

					// If player lands on top of the crate...
					if (player->GetState() == PlayerState::LANDING && difY > 0)
					{
						// Set player's y velocity to 0.
						player->GetBody()->SetLinearVelocity(b2Vec2(player->GetBody()->GetLinearVelocity().x, 0));

						// If it's a jump crate, launch the player in the air, change state to jumping and play the bounce sound.
						if ((crate->GetType() == CrateType::JUMP_METAL) || (crate->GetType() == CrateType::JUMP_WOOD))
						{
							player->GetBody()->ApplyForceToCenter(b2Vec2(0, 1000), true);
							player->SetState(PlayerState::JUMPING);
							audio_manager_->PlaySample(2);
						}
						// If it's a wooden crate, launch the player slightly in the air, set state to jumping, play the bounce and crate destroyed sounds, then destroy the crate.
						else if (crate->GetType() == CrateType::WOOD)
						{
							player->GetBody()->ApplyForceToCenter(b2Vec2(0, 500), true);
							player->SetState(PlayerState::JUMPING);
							audio_manager_->PlaySample(3);
							audio_manager_->PlaySample(2);
							crate->Destroy();
						}
					}
					// If the player kicks the crate...
					else if (player->GetState() == PlayerState::KICKING)
					{
						// If the crate is made of wood, destroy it and play the crate destroyed sound.
						if ((crate->GetType() == CrateType::WOOD) || (crate->GetType() == CrateType::JUMP_WOOD))
						{
							crate->Destroy();
							audio_manager_->PlaySample(3);
						}
					}

					// If the player hits the crate from below, and it's made of wood, destroy it and play the crate destroyed sound.
					if (difY < -2.5 && crate->GetType() == CrateType::WOOD)
					{
						crate->Destroy();
						audio_manager_->PlaySample(3);
					}
				}
				
				// If the player collides with a coin...
				if (coin)
				{
					// Increase the score, set the coin to collected, and play the coin collected sound if it isn't already collected.
					if (!coin->GetCollected())
					{
						score_ += 1;
						coin->SetCollected(true);
						audio_manager_->PlaySample(8);
					}
					
				}

				// If the player collides with a sawblade, kill them if they're not already dead then play the death sound.
				if (sawblade && player->GetState() != PlayerState::DEAD)
				{
					player->SetDead();
					audio_manager_->PlaySample(5);
				}

				// If the player collides with the crusher and isn't dead...
				if (crusher && player->GetState() != PlayerState::DEAD)
				{
					float difY;
					difY = player->GetBody()->GetPosition().y + player_half_height_ - crusher->GetBody()->GetPosition().y;

					// If hitting bottom of the crusher, die and play death sound.
					if (difY < -crusher_half_height_ && crusher->GetCrushing())
					{
						player->SetDead();
						audio_manager_->PlaySample(5);
					}
					else if (difY > 0) // Otherwise if above, land on the crusher.
					{
						if (player->GetState() == PlayerState::FALLING)
						{
							player->SetState(PlayerState::LANDING);
						}
					}
				}
			}
		}

		// Get next contact point.
		contact = contact->GetNext();
	}
}

void Level::RenderHud()
{
	// Render the remaining lives, time passed, and coins collected at the top of the screen.
	font_->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_->width() * 0.05f, platform_->height() * 0.05f, 0.0f),
		1.0f,
		0xffffffff,
		gef::TJ_LEFT,
		"LIVES: %i",
		player_.GetLives());

	font_->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_->width() * 0.5f, platform_->height() * 0.05f, 0.0f),
		1.0f,
		0xffffffff,
		gef::TJ_CENTRE,
		"TIME: %.1fs",
		timer_);

	font_->RenderText(
		sprite_renderer_,
		gef::Vector4(platform_->width() * 0.95f, platform_->height() * 0.05f, 0.0f),
		1.0f,
		0xffffffff,
		gef::TJ_RIGHT,
		"COINS: %i",
		score_);
}
