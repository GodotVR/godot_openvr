////////////////////////////////////////////////////////////////////////////////////////////////
// GDNative module that exposes some OpenVR module options to Godot

#ifndef OPENVR_SKELETON_H
#define OPENVR_SKELETON_H

#include "openvr_data.h"
#include <Skeleton3D.hpp>
#include <String.hpp>
#include <Transform3D.hpp>

#include <stdlib.h>

namespace godot {
class OpenVRSkeleton : public Skeleton3D {
	GODOT_CLASS(OpenVRSkeleton, Skeleton3D)

private:
	openvr_data *ovr;

	String action;
	int action_idx;
	int on_hand;

	bool is_active;
	bool keep_bones;

	struct bone {
		vr::BoneIndex_t parent;
		char name[256];
		Transform3D rest_transform;
		Transform3D pose_transform;
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

	bool get_keep_bones() const;
	void set_keep_bones(bool p_keep_bones);

	int get_motion_range() const;
	void set_motion_range(int p_motion_range);

	bool get_is_active() const;
};
} // namespace godot

#endif /* !OPENVR_SKELETON_H */
