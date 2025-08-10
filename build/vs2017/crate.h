#pragma once
#include "game_object.h"
#include "graphics/renderer_3d.h"
#include "primitive_builder.h"
#include "box2d/box2d.h"
#include "coin.h"

// Different types of crates
// Wood - destructible, contains 3 coins
// Metal - indestructible
// Jump Wood - destructible, contains 1 coin, launches the player into the air
// Jump Metal - indestructible, launches the player into the air
// Destroyed - destroyed state
enum class CrateType
{
	WOOD,
	METAL,
	JUMP_WOOD,
	JUMP_METAL,
	DESTROYED
};

class Crate :
	public GameObject
{
public:
	Crate();

	// Functions for updating, initialising and reseting the crate.
	void Update(float frame_time);
	void Init(PrimitiveBuilder* primitive_builder, b2World* world);
	void Reset();

	// Functions to render the planks and coins.
	void RenderPlanks(gef::Renderer3D* renderer_3d);
	void RenderCoins(gef::Renderer3D* renderer_3d);

	// Function to destroy the crate.
	void Destroy();

	// To update the physics of the planks and coins after the crate is destroyed.
	void UpdateDestroyedSimulation();

	// Getter and setter for the crate's type.
	void SetType(CrateType type)
	{
		type_ = type;
	};
	CrateType GetType() 
	{
		return type_;
	};

private:
	// The current crate type, and the initial great type for when the crate needs to be reset.
	CrateType type_;
	CrateType initial_type_;

	// Boolean for storing whether the crate has been destroyed. Used alongside the destroyed state for managing the destruction of the crate and release of coins.
	bool destroyed_;

	// Float for holding time passed. Used for a delay in enabling collisions of coins.
	float timer_;

	// Planks to be released when the crate is destroyed.
	GameObject planks_[4];
	int plank_count_ = 4;

	// Coins to be released when the crate is destroyed.
	Coin coins_[3];
	int coin_count_;
};

