#include "OpenVRSkeleton.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <string.h>

using namespace godot;

void OpenVRSkeleton::_bind_methods() {
	// ClassDB::bind_method(D_METHOD("_process"), &OpenVRSkeleton::_process);

	ClassDB::bind_method(D_METHOD("get_action"), &OpenVRSkeleton::get_action);
	ClassDB::bind_method(D_METHOD("set_action", "action"), &OpenVRSkeleton::set_action);
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "action"), "set_action", "get_action");

	ClassDB::bind_method(D_METHOD("get_keep_bones"), &OpenVRSkeleton::get_keep_bones);
	ClassDB::bind_method(D_METHOD("set_keep_bones", "keep_bones"), &OpenVRSkeleton::set_keep_bones);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "keep_bones"), "set_keep_bones", "get_keep_bones");

	ClassDB::bind_method(D_METHOD("get_motion_range"), &OpenVRSkeleton::get_motion_range);
	ClassDB::bind_method(D_METHOD("set_motion_range", "motion_range"), &OpenVRSkeleton::set_motion_range);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "motion_range", PROPERTY_HINT_ENUM, "With controller,Without controller"), "set_motion_range", "get_motion_range");

	ClassDB::bind_method(D_METHOD("is_active"), &OpenVRSkeleton::get_is_active);
}

void OpenVRSkeleton::cleanup_bones() {
	bone_count = 0;
	if (bones != nullptr) {
		::free(bones);
		bones = nullptr;
	}
}

void OpenVRSkeleton::_process(double delta) {
	vr::EVRInputError err;
	is_active = false;

	// TODO change how this works
	// vr::VRActionHandle_t handle = ovr->get_custom_handle(action_idx);
	vr::VRActionHandle_t handle = vr::k_ulInvalidActionHandle;
	if (handle == vr::k_ulInvalidActionHandle) {
		return;
	}

	// get our current status
	vr::InputSkeletalActionData_t data;
	err = vr::VRInput()->GetSkeletalActionData(handle, &data, sizeof(data));
	if (err == vr::VRInputError_NoData) {
		// no data yet, just exit
		return;
	} else if (err != vr::VRInputError_None) {
		Array arr;
		arr.push_back(Variant((int64_t)err));
		UtilityFunctions::print(String("Couldn't retrieve skeletal action data, err: {0}").format(arr));
		return;
	}
	if (!data.bActive) {
		// not active, don't continue
		return;
	}

	if (bone_count == -1) {
		// we failed on a previous pass, don't try again...
		return;
	} else if (bone_count == 0) {
		// first time we get our handle? lets init our bone data
		uint32_t new_bone_count;
		uint32_t curr_bone_count = get_bone_count();
		vr::BoneIndex_t parent_indices[256];
		vr::VRBoneTransform_t reference_transforms[256];

		// get our bone count
		err = vr::VRInput()->GetBoneCount(handle, &new_bone_count);
		if (err != vr::VRInputError_None) {
			bone_count = -1; // prevent doing this again...
			Array arr;
			arr.push_back(Variant((int64_t)err));
			UtilityFunctions::print(String("Couldn't retrieve bone count, err: {0}").format(arr));
			return;
		} else if (new_bone_count > 255) {
			// I'm a lazy son of a b****, can't be bothered to allocate buffers, we won't have more then 255 bones right?
			bone_count = -1; // prevent doing this again...
			Array arr;
			arr.push_back(Variant((int64_t)new_bone_count));
			UtilityFunctions::print(String("Too many bones: {0}").format(arr));
			return;
		}

		if (keep_bones && (new_bone_count == curr_bone_count)) {
			// get our bone information that was preconfigured
			bones = (bone *)malloc(sizeof(bone) * new_bone_count);
			if (bones == nullptr) {
				bone_count = -1; // prevent doing this again...
				UtilityFunctions::print(String("Couldn't allocate memory"));
				return;
			}
			bone_count = new_bone_count;

			for (vr::BoneIndex_t i = 0; i < bone_count; i++) {
				bones[i].parent = get_bone_parent(i);
				// no need to get the name...
				bones[i].rest_transform = get_bone_rest(i);
				bones[i].pose_transform = get_bone_pose(i);
			}
		} else {
			// get our parent index data
			err = vr::VRInput()->GetBoneHierarchy(handle, parent_indices, 256);
			if (err != vr::VRInputError_None) {
				bone_count = -1; // prevent doing this again...
				Array arr;
				arr.push_back(Variant((int64_t)err));
				UtilityFunctions::print(String("Couldn't retrieve parent indices, err: {0}").format(arr));
				return;
			}

			// get our reference transforms
			err = vr::VRInput()->GetSkeletalReferenceTransforms(handle, transform_space, reference_pose, reference_transforms, 256);
			if (err != vr::VRInputError_None) {
				bone_count = -1; // prevent doing this again...
				Array arr;
				arr.push_back(Variant((int64_t)err));
				UtilityFunctions::print(String("Couldn't retrieve reference poses, err: {0}").format(arr));
				return;
			}

			// lets build our bone structure
			bones = (bone *)malloc(sizeof(bone) * new_bone_count);
			if (bones == nullptr) {
				bone_count = -1; // prevent doing this again...
				UtilityFunctions::print(String("Couldn't allocate memory"));
				return;
			}
			bone_count = new_bone_count;

			// remove our current bones (if any)
			clear_bones();

			// add our bones
			for (vr::BoneIndex_t i = 0; i < bone_count; i++) {
				bones[i].parent = parent_indices[i];

				// get the bone name
				err = vr::VRInput()->GetBoneName(handle, i, bones[i].name, 256);
				if (err != vr::VRInputError_None) {
					strcpy(bones[i].name, "Error");
					Array arr;
					arr.push_back(Variant((int64_t)err));
					UtilityFunctions::print(String("Couldn't retrieve bone name, err: {0}").format(arr));
				}

				// add our bone in Godot, note that for some reason our root node is named Root which it doesn't really like...
				add_bone(String(bones[i].name).to_lower());
				if (bones[i].parent >= 0) {
					// our parent should always be before us...
					set_bone_parent(i, bones[i].parent);
				}

				// get our reference transform
				ovr->transform_from_bone(bones[i].rest_transform, &reference_transforms[i]);

				// now we need the difference...
				set_bone_rest(i, bones[i].rest_transform);
				set_bone_pose_position(i, Vector3());
				set_bone_pose_rotation(i, Quaternion());
			}
		}
	}

	// now populate our bone data
	vr::VRBoneTransform_t bone_transforms[256];
	err = vr::VRInput()->GetSkeletalBoneData(handle, transform_space, motion_range, bone_transforms, bone_count);
	if (err != vr::VRInputError_None) {
		Array arr;
		arr.push_back(Variant((int64_t)err));
		UtilityFunctions::print(String("Couldn't retrieve skeletal bone transform data, err: {0}").format(arr));
		return;
	}

	for (vr::BoneIndex_t i = 0; i < bone_count; i++) {
		// convert to godot transforms, might need to inverse with parent or rest.. dunne know yet
		ovr->transform_from_bone(bones[i].pose_transform, &bone_transforms[i]);

		Transform3D pose_transform = bones[i].pose_transform;
		pose_transform = bones[i].rest_transform.inverse() * pose_transform;

		// TODO This has to be rewritten, we should use the quarternion data as we get it.
		set_bone_pose_position(i, pose_transform.origin);
		set_bone_pose_rotation(i, pose_transform.basis.get_rotation_quat());
	}

	// I guess we're active...
	is_active = true;
}

OpenVRSkeleton::OpenVRSkeleton() {
	ovr = openvr_data::retain_singleton();
	action_idx = -1;
	is_active = false;
	keep_bones = true;
	bone_count = 0;
	bones = nullptr;
	//	transform_space = vr::VRSkeletalTransformSpace_Model;
	transform_space = vr::VRSkeletalTransformSpace_Parent;
	motion_range = vr::VRSkeletalMotionRange_WithController;
	reference_pose = vr::VRSkeletalReferencePose_BindPose;
}

OpenVRSkeleton::~OpenVRSkeleton() {
	cleanup_bones();

	if (ovr != nullptr) {
		ovr->release();
		ovr = nullptr;
	}
}

String OpenVRSkeleton::get_action() const {
	return action;
}

void OpenVRSkeleton::set_action(String p_action) {
	action = p_action;

	// TODO change how this works
	// action_idx = ovr->register_custom_action(p_action);
	action_idx = -1;

	cleanup_bones();
}

bool OpenVRSkeleton::get_keep_bones() const {
	return keep_bones;
}

void OpenVRSkeleton::set_keep_bones(bool p_keep_bones) {
	keep_bones = p_keep_bones;
}

int OpenVRSkeleton::get_motion_range() const {
	return motion_range;
}

void OpenVRSkeleton::set_motion_range(int p_motion_range) {
	if (p_motion_range < 0) {
		// out of bounds
		return;
	} else if (p_motion_range > vr::VRSkeletalMotionRange_WithoutController) {
		// out of bounds
		return;
	}
	motion_range = (vr::EVRSkeletalMotionRange)p_motion_range;
}

bool OpenVRSkeleton::get_is_active() const {
	return is_active;
}
