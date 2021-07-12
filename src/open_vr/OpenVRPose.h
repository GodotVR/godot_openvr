////////////////////////////////////////////////////////////////////////////////////////////////
// GDNative module that exposes some OpenVR module options to Godot

#ifndef OPENVR_POSE_H
#define OPENVR_POSE_H

#include "openvr_data.h"
#include <Node3D.hpp>
#include <String.hpp>
#include <XRServer.hpp>

namespace godot {
class OpenVRPose : public Node3D {
	GODOT_CLASS(OpenVRPose, Node3D)

private:
	openvr_data *ovr;
	XRServer *server;

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
