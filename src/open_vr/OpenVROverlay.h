////////////////////////////////////////////////////////////////////////////////////////////////
// GDNative module that exposes overlay functions from OpenVR to Godot

#ifndef OPENVR_OVERLAY_H
#define OPENVR_OVERLAY_H

#include "godot_openvr.h"
#include <ProjectSettings.hpp>
#include <Viewport.hpp>

namespace godot {
class OpenVROverlay : public Viewport {
	GODOT_CLASS(OpenVROverlay, Viewport)

private:
	openvr_data *ovr;
	vr::VROverlayHandle_t overlay;
	int overlay_id;

	real_t overlay_width_in_meters;
	bool overlay_visible;

public:
	static void _register_methods();

	OpenVROverlay();
	~OpenVROverlay();

	void _init();
	void _ready();
	void _exit_tree();

	real_t get_overlay_width_in_meters() const;
	void set_overlay_width_in_meters(real_t p_new_size);

	bool is_overlay_visible() const;
	void set_overlay_visible(bool p_visible);

	bool track_relative_to_device(vr::TrackedDeviceIndex_t p_tracked_device_index, Transform3D p_transform);
	bool overlay_position_absolute(Transform3D p_transform);
};
} // namespace godot

#endif /* !OPENVR_OVERLAY_H */
