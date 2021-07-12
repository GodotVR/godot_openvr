#include "OpenVRPose.h"

using namespace godot;

void OpenVRPose::_register_methods() {
	register_method("_process", &OpenVRPose::_process);

	register_method("get_action", &OpenVRPose::get_action);
	register_method("set_action", &OpenVRPose::set_action);
	register_property<OpenVRPose, String>("action", &OpenVRPose::set_action, &OpenVRPose::get_action, String());

	register_method("is_active", &OpenVRPose::get_is_active);

	register_method("get_on_hand", &OpenVRPose::get_on_hand);
	register_method("set_on_hand", &OpenVRPose::set_on_hand);
	register_property<OpenVRPose, int>("on_hand", &OpenVRPose::set_on_hand, &OpenVRPose::get_on_hand, 0, GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT, GODOT_PROPERTY_HINT_ENUM, "any,left,right");
}

void OpenVRPose::_init() {
	// nothing to do here
}

void OpenVRPose::_process(float delta) {
	is_active = false;

	vr::InputPoseActionData_t pose_data;
	if (ovr->get_custom_pose_data(action_idx, &pose_data, on_hand)) {
		is_active = pose_data.bActive && pose_data.pose.bPoseIsValid;

		if (is_active) {
			// printf("Pose is active\n");

			float world_scale = godot::XRServer::get_singleton()->get_world_scale();
			Transform3D transform = ovr->transform_from_matrix(&pose_data.pose.mDeviceToAbsoluteTracking, world_scale);
			set_transform(server->get_reference_frame() * transform);
		} else {
			// printf("Pose is inactive\n");
		}
	}
}

OpenVRPose::OpenVRPose() {
	ovr = openvr_data::retain_singleton();
	server = XRServer::get_singleton();
	action_idx = -1;
	is_active = false;
	on_hand = 0;
}

OpenVRPose::~OpenVRPose() {
	if (ovr != NULL) {
		ovr->release();
		ovr = NULL;
	}
}

String OpenVRPose::get_action() const {
	return action;
}

void OpenVRPose::set_action(String p_action) {
	action = p_action;
	action_idx = ovr->register_custom_action(p_action);
}

bool OpenVRPose::get_is_active() const {
	return is_active;
}

int OpenVRPose::get_on_hand() const {
	return on_hand;
}

void OpenVRPose::set_on_hand(int p_hand) {
	if (p_hand < 0 || p_hand > 2) {
		return;
	}

	on_hand = p_hand;
}
