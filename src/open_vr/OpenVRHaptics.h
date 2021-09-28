////////////////////////////////////////////////////////////////////////////////////////////////
// GDNative module that exposes some OpenVR module options to Godot

#ifndef OPENVR_HAPTICS_H
#define OPENVR_HAPTICS_H

#include "openvr_data.h"
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot {
class OpenVRHaptics : public Node3D {
	GDCLASS(OpenVRHaptics, Node3D)

private:
	openvr_data *ovr;

	String action;
	int action_idx;

	int on_hand;

	float duration;
	float frequency;
	float amplitude;

protected:
	static void _bind_methods();

public:
	OpenVRHaptics();
	~OpenVRHaptics();

	String get_action() const;
	void set_action(String p_action);

	int get_on_hand() const;
	void set_on_hand(int p_hand);

	float get_duration() const;
	void set_duration(const float p_duration);

	float get_frequency() const;
	void set_frequency(const float p_frequency);

	float get_amplitude() const;
	void set_amplitude(const float p_amplitude);

	void trigger_pulse();
};
} // namespace godot

#endif /* !OPENVR_HAPTICS_H */
