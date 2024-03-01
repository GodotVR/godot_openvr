////////////////////////////////////////////////////////////////////////////////////////////////
// GDNative module that exposes overlay functions from OpenVR to Godot

#ifndef OPENVR_OVERLAY_H
#define OPENVR_OVERLAY_H

#include "openvr_data.h"
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/classes/sub_viewport.hpp>

namespace godot {
class OpenVROverlay : public SubViewport {
	GDCLASS(OpenVROverlay, SubViewport)

private:
	openvr_data *ovr;
	vr::VROverlayHandle_t overlay;
	int overlay_id;

	real_t overlay_width_in_meters;
	bool overlay_visible;

protected:
	static void _bind_methods();

public:
	OpenVROverlay();
	~OpenVROverlay();

	virtual void _ready() override;
	virtual void _exit_tree() override;

	real_t get_overlay_width_in_meters() const;
	void set_overlay_width_in_meters(real_t p_new_size);

	bool is_overlay_visible() const;
	void set_overlay_visible(bool p_visible);

	bool track_relative_to_device(vr::TrackedDeviceIndex_t p_tracked_device_index, Transform3D p_transform);
	bool overlay_position_absolute(Transform3D p_transform);
};
} // namespace godot

#endif /* !OPENVR_OVERLAY_H */
