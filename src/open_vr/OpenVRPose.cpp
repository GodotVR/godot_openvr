#include "OpenVRPose.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void OpenVRPose::_bind_methods() {
	// ClassDB::bind_method(D_METHOD("_process"), &OpenVRPose::_process);

	ClassDB::bind_method(D_METHOD("get_action"), &OpenVRPose::get_action);
	ClassDB::bind_method(D_METHOD("set_action", "action"), &OpenVRPose::set_action);
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "action"), "set_action", "get_action");

	ClassDB::bind_method(D_METHOD("is_active"), &OpenVRPose::get_is_active);

	ClassDB::bind_method(D_METHOD("get_on_hand"), &OpenVRPose::get_on_hand);
	ClassDB::bind_method(D_METHOD("set_on_hand", "hand"), &OpenVRPose::set_on_hand);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "on_hand", PROPERTY_HINT_ENUM, "any,left,right"), "set_on_hand", "get_on_hand");
}

void OpenVRPose::_process(double delta) {
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
	if (ovr != nullptr) {
		ovr->release();
		ovr = nullptr;
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
