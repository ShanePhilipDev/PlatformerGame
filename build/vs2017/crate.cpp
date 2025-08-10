#include "crate.h"

// Constructor
Crate::Crate()
{
	// Default type is wood. Crate is not destroyed and timer starts at 0.
	SetType(CrateType::WOOD);
	destroyed_ = false;
	timer_ = 0;
}

void Crate::Update(float frame_time)
{
	// If the crate's type is destroyed, but the destroyed boolean hasn't been set true yet. This should run only one time after the crate has been destroyed.
	if (GetType() == CrateType::DESTROYED && !destroyed_)
	{
		// Set destroyed boolean to true.
		destroyed_ = true;

		// For each plank...
		for (int i = 0; i < plank_count_; i++)
		{
			// Force to be applied to each plank. Each plank flies off in a different direction.
			b2Vec2 force;
			switch (i)
			{
			case 0:
				force = b2Vec2(-100, 100);
				break;
			case 1:
				force = b2Vec2(-100, 200);
				break;
			case 2:
				force = b2Vec2(100, 200);
				break;
			case 3:
				force = b2Vec2(100, 100);
				break;
			}

			// Enable the plank's physics body, applies the above defined force, and applies torque to make it rotate.
			planks_[i].GetBody()->SetEnabled(true);
			planks_[i].GetBody()->ApplyForceToCenter(force, true);
			planks_[i].GetBody()->ApplyTorque(20, true);
			
		}

		// For each coin...
		for (int i = 0; i < coin_count_; i++)
		{
			// Force to be applied to each coin. Each coin flies off in a different direction.
			b2Vec2 force;
			switch (i)
			{
			case 0:
				force = b2Vec2(0, 150);
				break;
			case 1:
				force = b2Vec2(-25, 150);
				break;
			case 2:
				force = b2Vec2(25, 150);
				break;
			}
			
			// Enable the coin's physics body and applies the above defined force.
			coins_[i].GetBody()->SetEnabled(true);
			coins_[i].GetBody()->ApplyForceToCenter(force, true);
		}
	}

	// If the crate is destroyed...
	if (destroyed_)
	{
		// Increment the timer by the frame time.
		timer_ += frame_time;

		// After 0.2 seconds, the coins will have their sensor status set to false so that they can colide with other coins or the environment.
		if (timer_ > 0.2) 
		{
			for (int i = 0; i < coin_count_; i++)
			{
				coins_[i].GetBody()->GetFixtureList()->SetSensor(false);
			}
		}

	}

	// Checks if each coin has been collected, and disables them if they have been.
	for (int i = 0; i < coin_count_; i++)
	{
		if (coins_[i].GetCollected())
		{
			coins_[i].GetBody()->GetFixtureList()->SetSensor(true);
			coins_[i].GetBody()->SetEnabled(false);
		}
	}

}

void Crate::Init(PrimitiveBuilder* primitive_builder, b2World* world)
{
	// The crate type determines how many coins are contained within the crate. The crate type should have been defined before calling this function, otherwise it will default to a wooden crate.
	switch (type_)
	{
	case CrateType::WOOD:
		coin_count_ = 3;
		break;
	case CrateType::JUMP_WOOD:
		coin_count_ = 1;
		break;
	default:
		coin_count_ = 0;
		break;
	}

	// Save the initial type to be used when reseting the crate.
	initial_type_ = type_;

	// The half dimensions of a plank.
	gef::Vector4 plank_half_dimensions(0.1f, 0.4f, 0.02f);

	// Create a physics body for the plank. It will be dynamic and its position is the crate's position.
	b2BodyDef plank_body_def;
	plank_body_def.type = b2_dynamicBody;
	plank_body_def.position = b2Vec2(GetBody()->GetPosition().x, GetBody()->GetPosition().y);

	// Create the shape for the plank.
	b2PolygonShape plank_shape;
	plank_shape.SetAsBox(plank_half_dimensions.x(), plank_half_dimensions.y());

	// Create the planks fixture.
	b2FixtureDef plank_fixture_def;
	plank_fixture_def.shape = &plank_shape;
	plank_fixture_def.density = 1.0f;

	// Setup each plank.
	for (int i = 0; i < plank_count_; i++)
	{
		// Create a mesh for the plank based on its defined dimensions.
		planks_[i].set_mesh(primitive_builder->CreateBoxMesh(plank_half_dimensions));

		// Create a connection between the rigid body and plank.
		plank_body_def.userData.pointer = reinterpret_cast<uintptr_t>(&planks_[i]);
		planks_[i].SetBody(plank_body_def, world);

		// Create the fixture on the rigid body.
		planks_[i].GetBody()->CreateFixture(&plank_fixture_def);

		// Disables the body and sets it to a sensor (so it doesn't stop on collision with other objects).
		planks_[i].GetBody()->SetEnabled(false);
		planks_[i].GetBody()->GetFixtureList()->SetSensor(true);

		// Update visuals from simulation data.
		planks_[i].UpdateFromSimulation();
	}

	// The half dimensions of a coin.
	gef::Vector4 coin_half_dimensions(0.3f, 0.3f, 0.0f);

	// Create a physics body for the coin. It will be dynamic and its position is the crate's position.
	b2BodyDef coin_body_def;
	coin_body_def.type = b2_dynamicBody;
	coin_body_def.position = b2Vec2(GetBody()->GetPosition().x, GetBody()->GetPosition().y);

	// Create the shape for the coin.
	b2PolygonShape coin_shape;
	coin_shape.SetAsBox(coin_half_dimensions.x(), coin_half_dimensions.y());

	// Create the coin's fixture.
	b2FixtureDef coin_fixture_def;
	coin_fixture_def.shape = &coin_shape;
	coin_fixture_def.density = 1.0f;

	// Setup each coin.
	for (int i = 0; i < coin_count_; i++)
	{
		// Create a mesh for the coin based on its defined dimensions.
		coins_[i].set_mesh(primitive_builder->CreateBoxMesh(coin_half_dimensions));

		// Create a connection between the rigid body and coin.
		coin_body_def.userData.pointer = reinterpret_cast<uintptr_t>(&coins_[i]);
		coins_[i].SetBody(coin_body_def, world);

		// Create the fixture on the rigid body.
		coins_[i].GetBody()->CreateFixture(&coin_fixture_def);

		// Disables the body, sets it to a sensor and sets fixed rotation to true so that it doesn't rotate.
		coins_[i].GetBody()->SetEnabled(false);
		coins_[i].GetBody()->GetFixtureList()->SetSensor(true);
		coins_[i].GetBody()->SetFixedRotation(true);

		// Update visuals from simulation data.
		coins_[i].UpdateFromSimulation();
	}
}

void Crate::Reset()
{
	// Reverts relevant variables back to their initial value.
	type_ = initial_type_;
	destroyed_ = false;
	timer_ = 0;
	GetBody()->GetFixtureList()->SetSensor(false);

	// For each plank...
	for (int i = 0; i < plank_count_; i++)
	{
		// Sets its velocity to 0, reset its position, disable it, then reflect these changes in the box2d simulation.
		planks_[i].GetBody()->SetLinearVelocity(b2Vec2(0, 0));
		planks_[i].GetBody()->SetTransform(b2Vec2(GetBody()->GetPosition().x, GetBody()->GetPosition().y), 0);
		planks_[i].GetBody()->SetEnabled(false);
		planks_[i].UpdateFromSimulation();
	}

	// For each coin...
	for (int i = 0; i < coin_count_; i++)
	{
		// Set its velocity to 0, reset its position, disables it, set it to be a sensor, set it to be uncollected, then reflect these changes in the box2d simulation.
		coins_[i].GetBody()->SetLinearVelocity(b2Vec2(0, 0));
		coins_[i].GetBody()->SetTransform(b2Vec2(GetBody()->GetPosition().x, GetBody()->GetPosition().y), 0);
		coins_[i].GetBody()->SetEnabled(false);
		coins_[i].GetBody()->GetFixtureList()->SetSensor(true);
		coins_[i].SetCollected(false);
		coins_[i].UpdateFromSimulation();
	}
}

void Crate::RenderPlanks(gef::Renderer3D* renderer_3d)
{
	// Render the destroyed crate's planks.
	for (int i = 0; i < plank_count_; i++)
	{
		renderer_3d->DrawMesh(planks_[i]);
	}
}

void Crate::RenderCoins(gef::Renderer3D* renderer_3d)
{
	// Render coins released from crate if they have not been collected.
	for (int i = 0; i < coin_count_; i++)
	{
		if (!coins_[i].GetCollected())
		{
			renderer_3d->DrawMesh(coins_[i]);
		}
	}
}

void Crate::Destroy()
{
	// Set the crate's type to be the destroyed state, and change it into a sensor so that the player can pass through its box2d collision box.
	SetType(CrateType::DESTROYED);
	GetBody()->GetFixtureList()->SetSensor(true);
}

void Crate::UpdateDestroyedSimulation()
{
	// Update each plank based on the box2d simulation.
	for (int i = 0; i < plank_count_; i++)
	{
		planks_[i].UpdateFromSimulation();
	}

	// Update each coin based on the box2d simulation.
	for (int i = 0; i < coin_count_; i++)
	{
		coins_[i].UpdateFromSimulation();
	}
}
