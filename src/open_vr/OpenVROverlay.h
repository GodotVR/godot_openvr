////////////////////////////////////////////////////////////////////////////////////////////////
// GDNative module that exposes overlay functions from OpenVR to Godot

#ifndef OPENVR_OVERLAY_H
#define OPENVR_OVERLAY_H

#include "godot_openvr.h"
#include <Reference.hpp>

namespace godot {

class OpenVROverlay : public Reference {
	GODOT_CLASS(OpenVROverlay, Reference)

private:
	openvr_data *ovr;

public:
	static void _register_methods();

	void _init();

	OpenVROverlay();
	~OpenVROverlay();

	bool create_overlay(String p_overlay_key, String p_overlay_name);
	bool destroy_overlay();

	real_t get_overlay_width_in_meters() const;
	void set_overlay_width_in_meters(real_t p_new_size);

	bool is_overlay_visible() const;
	void set_overlay_visible(bool p_visible);
	void show_overlay();
	void hide_overlay();

	bool track_relative_to_device(vr::TrackedDeviceIndex_t p_tracked_device_index, Transform p_transform);
	bool overlay_position_absolute(Transform p_transform);
};

}

/*
#ifdef __cplusplus
extern "C" {
#endif

GDCALLINGCONV godot_variant openvr_overlay_is_overlay_visible(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);
GDCALLINGCONV godot_variant openvr_overlay_show_overlay(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);
GDCALLINGCONV godot_variant openvr_overlay_hide_overlay(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);
GDCALLINGCONV godot_variant openvr_overlay_track_relative_to_device(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);
GDCALLINGCONV godot_variant openvr_overlay_position_absolute(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);

#ifdef __cplusplus
}
#endif
*/

#endif /* !OPENVR_OVERLAY_H */
