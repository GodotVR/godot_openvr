////////////////////////////////////////////////////////////////////////////////////////////////
// GDNative module that exposes some OpenVR module options to Godot

#ifndef OPENVR_CONFIG_H
#define OPENVR_CONFIG_H

#include "openvr_data.h"
#include <PackedVector3Array.hpp>
#include <RefCounted.hpp>
#include <String.hpp>
#include <Transform3D.hpp>
#include <XRServer.hpp>

namespace godot {
class OpenVRConfig : public RefCounted {
	GODOT_CLASS(OpenVRConfig, RefCounted)

private:
	openvr_data *ovr;
	XRServer *server;

public:
	static void _register_methods();

	void _init();

	OpenVRConfig();
	~OpenVRConfig();

	int get_application_type() const;
	void set_application_type(int p_type);

	int get_tracking_universe() const;
	void set_tracking_universe(int p_universe);

	String get_default_action_set() const;
	void set_default_action_set(const String p_name);

	void register_action_set(const String p_action_set);
	void set_active_action_set(const String p_action_set);
	void toggle_action_set_active(const String p_action_set, const bool p_is_active);
	bool is_action_set_active(const String p_action_set) const;

	bool play_area_available() const;
	PackedVector3Array get_play_area() const;

	float get_device_battery_percentage(vr::TrackedDeviceIndex_t p_tracked_device_index);
	bool is_device_charging(vr::TrackedDeviceIndex_t p_tracked_device_index);
};
} // namespace godot

#endif /* !OPENVR_CONFIG_H */
