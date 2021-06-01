////////////////////////////////////////////////////////////////////////////////////////////////
// GDNative module that exposes some OpenVR module options to Godot

#ifndef OPENVR_ACTION_H
#define OPENVR_ACTION_H

#include "openvr_data.h"
#include <Dictionary.hpp>
#include <Node3D.hpp>
#include <String.hpp>

namespace godot {
class OpenVRAction : public Node3D {
	GODOT_CLASS(OpenVRAction, Node3D)

private:
	openvr_data *ovr;

	String pressed_action;
	int pressed_action_idx;
	bool is_pressed;

	String analog_action;
	int analog_action_idx;
	Vector2 analog;

	int on_hand;

public:
	static void _register_methods();

	void _init();
	void _process(float delta);

	OpenVRAction();
	~OpenVRAction();

	String get_pressed_action() const;
	void set_pressed_action(const String p_action);
	bool get_is_pressed() const;

	String get_analog_action() const;
	void set_analog_action(const String p_action);
	Vector2 get_analog() const;

	int get_on_hand() const;
	void set_on_hand(int p_hand);
};
} // namespace godot

#endif /* !OPENVR_ACTION_H */
