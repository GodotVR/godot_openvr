////////////////////////////////////////////////////////////////////////////////////////////////
// GDNative module that exposes some OpenVR module options to Godot

#ifndef OPENVR_CONTROLLER_H
#define OPENVR_CONTROLLER_H

#include "openvr_data.h"
#include <godot_cpp/classes/xr_controller3d.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/string.hpp>

#include <vector>

namespace godot {
class OpenVRController : public XRController3D {
	GDCLASS(OpenVRController, XRController3D)

private:
	openvr_data *ovr;

	struct input_action {
		String name;
		int action_id;
		bool last_state;
	};

	std::vector<input_action> button_actions;

protected:
	static void _bind_methods();

public:
	virtual void _process(double delta) override;

	OpenVRController();
	~OpenVRController();

	PackedStringArray get_button_actions();
	void set_button_actions(PackedStringArray p_actions);

	Vector2 get_analog(String p_action);
	void trigger_haptic(String p_action, float p_duration, float p_frequency, float p_amplitude);
};
} // namespace godot

#endif /* !OPENVR_CONTROLLER_H */
