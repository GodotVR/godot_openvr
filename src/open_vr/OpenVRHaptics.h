////////////////////////////////////////////////////////////////////////////////////////////////
// GDNative module that exposes some OpenVR module options to Godot

#ifndef OPENVR_HAPTICS_H
#define OPENVR_HAPTICS_H

#include "openvr_data.h"
#include <Node3D.hpp>
#include <String.hpp>

namespace godot {
class OpenVRHaptics : public Node3D {
	GODOT_CLASS(OpenVRHaptics, Node3D)

private:
	openvr_data *ovr;

	String action;
	int action_idx;

	int on_hand;

	float duration;
	float frequency;
	float amplitude;

public:
	static void _register_methods();

	void _init();

	OpenVRHaptics();
	~OpenVRHaptics();

	String get_action() const;
	void set_action(String p_action);

	int get_on_hand() const;
	void set_on_hand(int p_hand);

	void trigger_pulse();
};
} // namespace godot

#endif /* !OPENVR_HAPTICS_H */
