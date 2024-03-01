#ifndef OPENVR_OVERLAY_CONTAINER_H
#define OPENVR_OVERLAY_CONTAINER_H

#include "openvr_data.h"
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/classes/sub_viewport_container.hpp>

namespace godot {
class OpenVROverlayContainer : public SubViewportContainer {
	GDCLASS(OpenVROverlayContainer, SubViewportContainer)

private:
	openvr_data *ovr;
	vr::VROverlayHandle_t overlay;
	int overlay_id;

	float overlay_width_in_meters;
	bool overlay_visible;

protected:
	static void _bind_methods();

public:
	OpenVROverlayContainer();
	~OpenVROverlayContainer();

	virtual void _ready() override;
	virtual void _exit_tree() override;

	float get_overlay_width_in_meters();
	void set_overlay_width_in_meters(float p_new_size);

	bool is_overlay_visible();
	void set_overlay_visible(bool p_visible);

	bool track_relative_to_device(vr::TrackedDeviceIndex_t p_tracked_device_index, Transform3D p_transform);
	bool overlay_position_absolute(Transform3D p_transform);
};
} // namespace godot

#endif /* !OPENVR_OVERLAY_CONTAINER_H */
