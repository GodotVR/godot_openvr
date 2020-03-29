////////////////////////////////////////////////////////////////////////////////////////////////
// GDNative module that exposes some OpenVR module options to Godot

#ifndef OPENVR_SKELETON_H
#define OPENVR_SKELETON_H

#include "openvr_data.h"
#include <Skeleton.hpp>
#include <String.hpp>
#include <Transform.hpp>

#include <stdlib.h>

namespace godot {

class OpenVRSkeleton : public Skeleton {
	GODOT_CLASS(OpenVRSkeleton, Skeleton)

private:
	openvr_data *ovr;

	String action;
	int action_idx;
	int on_hand;

	bool is_active;

	struct bone {
		vr::BoneIndex_t parent;
		char name[256];
		Transform rest_transform;
		Transform pose_transform;
	};

	vr::EVRSkeletalTransformSpace transform_space;
	vr::EVRSkeletalMotionRange motion_range;
	vr::EVRSkeletalReferencePose reference_pose;

	uint32_t bone_count;
	bone *bones;
	void cleanup_bones();

public:
	static void _register_methods();

	void _init();
	void _process(float delta);

	OpenVRSkeleton();
	~OpenVRSkeleton();

	String get_action() const;
	void set_action(String p_action);

	bool get_is_active() const;
};

} // namespace godot

#endif /* !OPENVR_SKELETON_H */
