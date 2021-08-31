////////////////////////////////////////////////////////////////////////////////////////////////
// GDNative module that exposes some OpenVR module options to Godot

#ifndef OPENVR_ACTION_H
#define OPENVR_ACTION_H

#include "openvr_data.h"
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot {
class OpenVRAction : public Node3D {
	GDCLASS(OpenVRAction, Node3D)

private:
	openvr_data *ovr;

	String pressed_action;
	int pressed_action_idx;
	bool is_pressed;

	String analog_action;
	int analog_action_idx;
	Vector2 analog;

	GDNativeInt on_hand;

protected:
	static void _bind_methods();

public:
	virtual void _process(double delta) override;

	OpenVRAction();
	~OpenVRAction();

	String get_pressed_action() const;
	void set_pressed_action(const String p_action);
	bool get_is_pressed() const;

	String get_analog_action() const;
	void set_analog_action(const String p_action);
	Vector2 get_analog() const;

	GDNativeInt get_on_hand() const;
	void set_on_hand(const GDNativeInt p_hand);
};
} // namespace godot

#endif /* !OPENVR_ACTION_H */
