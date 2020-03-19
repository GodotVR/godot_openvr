#include "OpenVRHaptics.h"

using namespace godot;

void OpenVRHaptics::_register_methods() {
	register_method("get_action", &OpenVRHaptics::get_action);
	register_method("set_action", &OpenVRHaptics::set_action);
	register_property<OpenVRHaptics, String>("action", &OpenVRHaptics::set_action, &OpenVRHaptics::get_action, String());

	register_method("get_on_hand", &OpenVRHaptics::get_on_hand);
	register_method("set_on_hand", &OpenVRHaptics::set_on_hand);
	register_property<OpenVRHaptics, int>("on_hand", &OpenVRHaptics::set_on_hand, &OpenVRHaptics::get_on_hand, 0, GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT, GODOT_PROPERTY_HINT_ENUM, "any,left,right");

	register_property<OpenVRHaptics, float>("duration", &OpenVRHaptics::duration, 1.0);
	register_property<OpenVRHaptics, float>("frequency", &OpenVRHaptics::frequency, 1.0);
	register_property<OpenVRHaptics, float>("amplitude", &OpenVRHaptics::amplitude, 1.0);

	register_method("trigger_pulse", &OpenVRHaptics::trigger_pulse);
}

void OpenVRHaptics::_init() {
	// nothing to do here
}

OpenVRHaptics::OpenVRHaptics() {
	ovr = openvr_data::retain_singleton();
	action_idx = -1;
	on_hand = 0;
	duration = 1.0f;
	frequency = 1.0f;
	amplitude = 1.0f;
}

OpenVRHaptics::~OpenVRHaptics() {
	if (ovr != NULL) {
		ovr->release();
		ovr = NULL;
	}
}

String OpenVRHaptics::get_action() const {
	return action;
}

void OpenVRHaptics::set_action(String p_action) {
	action = p_action;
	action_idx = ovr->register_custom_action(p_action);
}

int OpenVRHaptics::get_on_hand() const {
	return on_hand;
}

void OpenVRHaptics::set_on_hand(int p_hand) {
	if (p_hand < 0 || p_hand > 2) {
		return;
	}

	on_hand = p_hand;
}

void OpenVRHaptics::trigger_pulse() {
	ovr->trigger_custom_haptic(action_idx, 0.0f, duration, frequency, amplitude, on_hand);
}
