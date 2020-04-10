#include "OpenVRAction.h"

using namespace godot;

void OpenVRAction::_register_methods() {
	register_method("_process", &OpenVRAction::_process);

	register_method("get_pressed_action", &OpenVRAction::get_pressed_action);
	register_method("set_pressed_action", &OpenVRAction::set_pressed_action);
	register_property<OpenVRAction, String>("pressed_action", &OpenVRAction::set_pressed_action, &OpenVRAction::get_pressed_action, String());

	register_method("is_pressed", &OpenVRAction::get_is_pressed);

	register_signal<OpenVRAction>(String("pressed"), Dictionary());
	register_signal<OpenVRAction>(String("released"), Dictionary());

	register_method("get_analog_action", &OpenVRAction::get_analog_action);
	register_method("set_analog_action", &OpenVRAction::set_analog_action);
	register_property<OpenVRAction, String>("analog_action", &OpenVRAction::set_analog_action, &OpenVRAction::get_analog_action, String());

	register_method("get_analog", &OpenVRAction::get_analog);

	register_method("get_on_hand", &OpenVRAction::get_on_hand);
	register_method("set_on_hand", &OpenVRAction::set_on_hand);
	register_property<OpenVRAction, int>("on_hand", &OpenVRAction::set_on_hand, &OpenVRAction::get_on_hand, 0, GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT, GODOT_PROPERTY_HINT_ENUM, "any,left,right");
}

void OpenVRAction::_init() {
	// nothing to do here
}

void OpenVRAction::_process(float delta) {
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
	if (ovr != NULL) {
		ovr->release();
		ovr = NULL;
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

int OpenVRAction::get_on_hand() const {
	return on_hand;
}

void OpenVRAction::set_on_hand(int p_hand) {
	if (p_hand < 0 || p_hand > 2) {
		return;
	}

	on_hand = p_hand;
}
