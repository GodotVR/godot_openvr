#include "OpenVRHaptics.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void OpenVRHaptics::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_action"), &OpenVRHaptics::get_action);
	ClassDB::bind_method(D_METHOD("set_action", "action"), &OpenVRHaptics::set_action);
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "action"), "set_action", "get_action");

	ClassDB::bind_method(D_METHOD("get_on_hand"), &OpenVRHaptics::get_on_hand);
	ClassDB::bind_method(D_METHOD("set_on_hand", "hand"), &OpenVRHaptics::set_on_hand);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "on_hand", PROPERTY_HINT_ENUM, "any,left,right"), "set_on_hand", "get_on_hand");

	ClassDB::bind_method(D_METHOD("get_duration"), &OpenVRHaptics::get_duration);
	ClassDB::bind_method(D_METHOD("set_duration", "duration"), &OpenVRHaptics::set_duration);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "duration"), "set_duration", "get_duration");

	ClassDB::bind_method(D_METHOD("get_frequency"), &OpenVRHaptics::get_frequency);
	ClassDB::bind_method(D_METHOD("set_frequency", "frequency"), &OpenVRHaptics::set_frequency);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "frequency"), "set_frequency", "get_frequency");

	ClassDB::bind_method(D_METHOD("get_amplitude"), &OpenVRHaptics::get_amplitude);
	ClassDB::bind_method(D_METHOD("set_amplitude", "amplitude"), &OpenVRHaptics::set_amplitude);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "amplitude"), "set_amplitude", "get_amplitude");

	ClassDB::bind_method(D_METHOD("trigger_pulse"), &OpenVRHaptics::trigger_pulse);
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
	if (ovr != nullptr) {
		ovr->release();
		ovr = nullptr;
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

float OpenVRHaptics::get_duration() const {
	return duration;
}

void OpenVRHaptics::set_duration(const float p_duration) {
	duration = p_duration;
}

float OpenVRHaptics::get_frequency() const {
	return frequency;
}

void OpenVRHaptics::set_frequency(const float p_frequency) {
	frequency = p_frequency;
}

float OpenVRHaptics::get_amplitude() const {
	return amplitude;
}

void OpenVRHaptics::set_amplitude(const float p_amplitude) {
	amplitude = p_amplitude;
}

void OpenVRHaptics::trigger_pulse() {
	ovr->trigger_custom_haptic(action_idx, 0.0f, duration, frequency, amplitude, on_hand);
}
