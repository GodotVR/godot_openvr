////////////////////////////////////////////////////////////////////////////////////////////////
// GDNative module that exposes some OpenVR module options to Godot

#ifndef OPENVR_CONTROLLER_H
#define OPENVR_CONTROLLER_H

#include "openvr_data.h"
#include <PackedStringArray.hpp>
#include <String.hpp>
#include <XRController3D.hpp>

#include <vector>

namespace godot {
class OpenVRController : public XRController3D {
	GODOT_CLASS(OpenVRController, XRController3D)

private:
	openvr_data *ovr;

	struct input_action {
		String name;
		int action_id;
		bool last_state;
	};

	std::vector<input_action> button_actions;

protected:
public:
	static void _register_methods();

	void _init();
	void _process(float delta);

	OpenVRController();
	~OpenVRController();

	PackedStringArray get_button_actions();
	void set_button_actions(PackedStringArray p_actions);

	Vector2 get_analog(String p_action);
	void trigger_haptic(String p_action, float p_duration, float p_frequency, float p_amplitude);
};
} // namespace godot

#endif /* !OPENVR_CONTROLLER_H */
