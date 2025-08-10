#include "motion_clip_player.h"
#include <animation/animation.h>
#include <system/debug_log.h>

MotionClipPlayer::MotionClipPlayer() :
clip_(NULL),
anim_time_(0.0f),
playback_speed_(1.0f),
looping_(false)
{
}

void MotionClipPlayer::Init(const gef::SkeletonPose& bind_pose)
{
	pose_ = bind_pose;
}

bool MotionClipPlayer::Update(const float delta_time, const gef::SkeletonPose& bind_pose)
{
	bool finished = false;

	if (clip_)
	{
		// update the animation playback time
		anim_time_ += delta_time*playback_speed_;

		// check to see if the playback has reached the end of the animation
		if(anim_time_ > clip_->duration())
		{
			// if the animation is looping then wrap the playback time round to the beginning of the animation
			// other wise set the playback time to the end of the animation and flag that we have reached the end
			if(looping_)
				anim_time_ = std::fmodf(anim_time_, clip_->duration());
			else
			{
				anim_time_ = clip_->duration();
				finished = true;
			}
		}

		// add the clip start time to the playback time to calculate the final time
		// that will be used to sample the animation data
		float time = anim_time_+clip_->start_time();

		// sample the animation data at the calculated time
		// any bones that don't have animation data are set to the bind pose
		pose_.SetPoseFromAnim(*clip_, bind_pose, time);
	}
	else
	{
		// no animation associated with this player
		// just set the pose to the bind pose
		pose_ = bind_pose;
	}

	// return true if we have reached the end of the animation, always false when playback is looped
	return finished;
}

gef::Mesh* MotionClipPlayer::GetFirstMesh(gef::Scene* scene, gef::Platform* platform)
{
	gef::Mesh* mesh = NULL;

	if (scene)
	{
		// now check to see if there is any mesh data in the file, if so lets create a mesh from it
		if (scene->mesh_data.size() > 0)
			mesh = scene->CreateMesh(*platform, scene->mesh_data.front());
	}

	return mesh;
}

gef::Skeleton* MotionClipPlayer::GetFirstSkeleton(gef::Scene* scene)
{
	gef::Skeleton* skeleton = NULL;
	if (scene)
	{
		// check to see if there is a skeleton in the the scene file
		// if so, pull out the bind pose and create an array of matrices
		// that wil be used to store the bone transformations
		if (scene->skeletons.size() > 0)
			skeleton = scene->skeletons.front();
	}

	return skeleton;
}

gef::Animation* MotionClipPlayer::LoadAnimation(const char* anim_scene_filename, const char* anim_name, gef::Platform* platform)
{
	gef::Animation* anim = NULL;

	gef::Scene anim_scene;
	if (anim_scene.ReadSceneFromFile(*platform, anim_scene_filename))
	{
		// if the animation name is specified then try and find the named anim
		// otherwise return the first animation if there is one
		std::map<gef::StringId, gef::Animation*>::const_iterator anim_node_iter;
		if (anim_name)
			anim_node_iter = anim_scene.animations.find(gef::GetStringId(anim_name));
		else
			anim_node_iter = anim_scene.animations.begin();

		if (anim_node_iter != anim_scene.animations.end())
			anim = new gef::Animation(*anim_node_iter->second);
	}

	return anim;
}