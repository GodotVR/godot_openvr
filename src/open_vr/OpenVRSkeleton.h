////////////////////////////////////////////////////////////////////////////////////////////////
// GDNative module that exposes some OpenVR module options to Godot

#ifndef OPENVR_SKELETON_H
#define OPENVR_SKELETON_H

#include "openvr_data.h"
#include <godot_cpp/classes/skeleton3d.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/transform3d.hpp>

#include <stdlib.h>

namespace godot {
class OpenVRSkeleton : public Skeleton3D {
	GDCLASS(OpenVRSkeleton, Skeleton3D)

private:
	openvr_data *ovr = nullptr;

	String action;
	int action_idx = -1;
	int on_hand;

	bool is_active = false;
	bool keep_bones = true;

	struct bone {
		vr::BoneIndex_t parent;
		char name[256];
		Transform3D rest_transform;
		Transform3D pose_transform;
	};

	vr::EVRSkeletalTransformSpace transform_space = vr::VRSkeletalTransformSpace_Parent;
	vr::EVRSkeletalMotionRange motion_range = vr::VRSkeletalMotionRange_WithController;
	vr::EVRSkeletalReferencePose reference_pose = vr::VRSkeletalReferencePose_BindPose;

	uint32_t bone_count = 0;
	bone *bones = nullptr;
	void cleanup_bones();

protected:
	static void _bind_methods();

public:
	virtual void _process(double delta) override;

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
