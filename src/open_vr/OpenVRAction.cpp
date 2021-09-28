#include "OpenVRAction.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void OpenVRAction::_bind_methods() {
	// ClassDB::bind_method(D_METHOD("_process", "delta"), &OpenVRAction::_process);

	ClassDB::bind_method(D_METHOD("get_pressed_action"), &OpenVRAction::get_pressed_action);
	ClassDB::bind_method(D_METHOD("set_pressed_action", "action"), &OpenVRAction::set_pressed_action);
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "pressed_action"), "set_pressed_action", "get_pressed_action");

	ClassDB::bind_method(D_METHOD("is_pressed"), &OpenVRAction::get_is_pressed);

	ADD_SIGNAL(MethodInfo("pressed"));
	ADD_SIGNAL(MethodInfo("released"));

	ClassDB::bind_method(D_METHOD("get_analog_action"), &OpenVRAction::get_analog_action);
	ClassDB::bind_method(D_METHOD("set_analog_action", "action"), &OpenVRAction::set_analog_action);
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "analog_action"), "set_analog_action", "get_analog_action");

	ClassDB::bind_method(D_METHOD("get_analog"), &OpenVRAction::get_analog);

	ClassDB::bind_method(D_METHOD("get_on_hand"), &OpenVRAction::get_on_hand);
	ClassDB::bind_method(D_METHOD("set_on_hand", "hand"), &OpenVRAction::set_on_hand);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "on_hand", PROPERTY_HINT_ENUM, "any,left,right"), "set_on_hand", "get_on_hand");
}

void OpenVRAction::_process(double delta) {
	bool was_pressed = is_pressed;
	is_pressed = false;

	is_pressed = ovr->get_custom_digital_data(pressed_action_idx, on_hand);
	analog = ovr->get_custom_analog_data(analog_action_idx, on_hand);

	if (was_pressed && !is_pressed) {
		// signal release
		emit_signal("released");
	} else if (!was_pressed && is_pressed) {
		// signal pressed
		emit_signal("pressed");
	}
}

OpenVRAction::OpenVRAction() {
	ovr = openvr_data::retain_singleton();
	pressed_action_idx = -1;
	analog_action_idx = -1;
	on_hand = 0;
	is_pressed = false;
}

OpenVRAction::~OpenVRAction() {
	if (ovr != nullptr) {
		ovr->release();
		ovr = nullptr;
	}
}

String OpenVRAction::get_pressed_action() const {
	return pressed_action;
}

void OpenVRAction::set_pressed_action(const String p_action) {
	pressed_action = p_action;
	pressed_action_idx = ovr->register_custom_action(pressed_action);
}

bool OpenVRAction::get_is_pressed() const {
	return is_pressed;
}

String OpenVRAction::get_analog_action() const {
	return analog_action;
}

void OpenVRAction::set_analog_action(const String p_action) {
	analog_action = p_action;
	analog_action_idx = ovr->register_custom_action(analog_action);
}

godot::Vector2 OpenVRAction::get_analog() const {
	return analog;
}

GDNativeInt OpenVRAction::get_on_hand() const {
	return on_hand;
}

void OpenVRAction::set_on_hand(const GDNativeInt p_hand) {
	if (p_hand < 0 || p_hand > 2) {
		return;
	}

	on_hand = p_hand;
}
