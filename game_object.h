#ifndef _GAME_OBJECT_H
#define _GAME_OBJECT_H

#include <graphics/mesh_instance.h>
#include <box2d/Box2D.h>
#include <graphics/sprite.h>

// The types of game object that need to be differentiated.
enum OBJECT_TYPE
{
	PLAYER,
	GROUND,
	ENEMY,
	SAWBLADE,
	CRUSHER,
	CRATE,
	COIN,
	CHECKPOINT,
	NONE
};

class GameObject : public gef::MeshInstance
{
public:
	GameObject();

	// Update the mesh based on the box2d simulation.
	void UpdateFromSimulation();

	// Create a box2d body for the object.
	void SetBody(b2BodyDef body_def, b2World* world);

	// Getter for the body.
	b2Body* GetBody() { return body_; };

	// Setter and getter for the object's type.
	inline void set_type(OBJECT_TYPE type) { type_ = type; }
	inline OBJECT_TYPE type() { return type_; }
protected:
	// Holds the object's type.
	OBJECT_TYPE type_;

	// Pointer to hold the object's body.
	b2Body* body_;

};



#endif // _GAME_OBJECT_H