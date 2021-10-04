////////////////////////////////////////////////////////////////////////////////////////////////
// Our main XRInterface code for our OpenVR GDExtension module

#ifndef XR_INTERFACE_OPENVR_H
#define XR_INTERFACE_OPENVR_H

#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/xr_interface_extension.hpp>
#include <godot_cpp/classes/xr_server.hpp>
#include <godot_cpp/core/binder_common.hpp>

#include "openvr_data.h"

namespace godot {
class XRInterfaceOpenVR : public XRInterfaceExtension {
	GDCLASS(XRInterfaceOpenVR, XRInterfaceExtension);

protected:
	static void _bind_methods();

private:
	XRServer *xr_server = nullptr;
	openvr_data *ovr = nullptr;
	uint32_t width = 0;
	uint32_t height = 0;

	OS::VideoDriver video_driver = OS::VIDEO_DRIVER_VULKAN;
	int texture_id = 0;

public:
	// Properties
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

	// Functions
	virtual StringName _get_name() const override;
	virtual int64_t _get_capabilities() const override;

	virtual bool _is_initialized() const override;
	virtual bool _initialize() override;
	virtual void _uninitialize() override;

	virtual int64_t _get_tracking_status() const override;
	virtual void _trigger_haptic_pulse(const String &action_name, const StringName &tracker_name, double frequency, double amplitude, double duration_sec, double delay_sec) override;

	virtual Vector2 _get_render_target_size() override;
	virtual int64_t _get_view_count() override;
	virtual Transform3D _get_camera_transform() override;
	virtual Transform3D _get_transform_for_view(int64_t p_view, const Transform3D &p_cam_transform) override;
	virtual PackedFloat64Array _get_projection_for_view(int64_t p_view, double p_aspect, double p_z_near, double p_z_far) override;

	virtual void _commit_views(const RID &p_render_target, const Rect2 &p_screen_rect) override;

	virtual void _process() override;
	virtual void _notification(int64_t what) override;

	XRInterfaceOpenVR();
	~XRInterfaceOpenVR();
};
} // namespace godot

#endif /* !XR_INTERFACE_OPENVR_H */
