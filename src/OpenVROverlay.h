////////////////////////////////////////////////////////////////////////////////////////////////
// GDNative module that exposes overlay functions from OpenVR to Godot

#ifndef OPENVR_OVERLAY_H
#define OPENVR_OVERLAY_H

#include "GodotCalls.h"
#include "OVRCalls.h"

#ifdef __cplusplus
extern "C" {
#endif

GDCALLINGCONV void *openvr_overlay_constructor(godot_object *p_instance, void *p_method_data);
GDCALLINGCONV void openvr_overlay_destructor(godot_object *p_instance, void *p_method_data, void *p_user_data);
GDCALLINGCONV godot_variant openvr_overlay_create_overlay(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);
GDCALLINGCONV godot_variant openvr_overlay_destroy_overlay(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);
GDCALLINGCONV godot_variant openvr_overlay_get_overlay_width_in_meters(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);
GDCALLINGCONV godot_variant openvr_overlay_set_overlay_width_in_meters(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);
GDCALLINGCONV godot_variant openvr_overlay_is_overlay_visible(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);
GDCALLINGCONV godot_variant openvr_overlay_show_overlay(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);
GDCALLINGCONV godot_variant openvr_overlay_hide_overlay(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);
GDCALLINGCONV godot_variant openvr_overlay_track_relative_to_device(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);
GDCALLINGCONV godot_variant openvr_overlay_position_absolute(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);

#ifdef __cplusplus
}
#endif

#endif /* !OPENVR_OVERLAY_H */
