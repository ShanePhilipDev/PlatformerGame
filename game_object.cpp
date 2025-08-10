#include "game_object.h"
#include <system/debug_log.h>

GameObject::GameObject()
{
	set_type(NONE); // default type is none
}

//
// UpdateFromSimulation
// 
// Update the transform of this object from a physics rigid body
//
void GameObject::UpdateFromSimulation()
{
	if (body_)
	{
		// setup object rotation
		gef::Matrix44 object_rotation;
		object_rotation.RotationZ(body_->GetAngle());


		// setup the object translation
		gef::Vector4 object_translation(body_->GetPosition().x, body_->GetPosition().y, 0.0f);

		// build object transformation matrix
		gef::Matrix44 object_transform = object_rotation;

		// set final transformation
		object_transform.SetTranslation(object_translation);
		set_transform(object_transform);
	}
}

void GameObject::SetBody(b2BodyDef body_def, b2World* world)
{
	// create the game object's body
	body_ = world->CreateBody(&body_def);
}







