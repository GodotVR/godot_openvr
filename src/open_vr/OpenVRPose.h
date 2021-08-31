////////////////////////////////////////////////////////////////////////////////////////////////
// GDNative module that exposes some OpenVR module options to Godot

#ifndef OPENVR_POSE_H
#define OPENVR_POSE_H

#include "openvr_data.h"
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/xr_server.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot {
class OpenVRPose : public Node3D {
	GDCLASS(OpenVRPose, Node3D)

private:
	openvr_data *ovr;
	XRServer *server;

	String action;
	int action_idx;
	int on_hand;

	bool is_active;

protected:
	static void _bind_methods();

public:
	virtual void _process(double delta) override;

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
