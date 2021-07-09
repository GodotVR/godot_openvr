////////////////////////////////////////////////////////////////////////////////////////////////
// GDNative module that exposes some OpenVR module options to Godot

#ifndef OPENVR_POSE_H
#define OPENVR_POSE_H

#include "openvr_data.h"
#include <ARVRServer.hpp>
#include <Spatial.hpp>
#include <String.hpp>

namespace godot {
class OpenVRPose : public Spatial {
	GODOT_CLASS(OpenVRPose, Spatial)

private:
	openvr_data *ovr;
	ARVRServer *server;

	String action;
	int action_idx;
	int on_hand;

	bool is_active;

public:
	static void _register_methods();

	void _init();
	void _process(float delta);

	OpenVRPose();
	~OpenVRPose();

	String get_action() const;
	void set_action(String p_action);
	bool get_is_active() const;

	int get_on_hand() const;
	void set_on_hand(int p_hand);
};
} // namespace godot

#endif /* !OPENVR_POSE_H */
