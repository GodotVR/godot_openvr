////////////////////////////////////////////////////////////////////////////////////////////////
// Our main ARVRInterface code for our OpenVR GDNative module

#ifndef OVR_ARVR_INTERFACE_H
#define OVR_ARVR_INTERFACE_H

#include "GodotCalls.h"
#include "OVRCalls.h"
#include "OS.h"

extern const godot_arvr_interface_gdnative interface_struct;

// declare our public functions for our ARVR Interface
#ifdef __cplusplus
extern "C" {
#endif

void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options *p_options);
void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options *p_options);
void GDN_EXPORT godot_gdnative_singleton();
void GDN_EXPORT godot_nativescript_init(void *p_handle);
void GDN_EXPORT *godot_arvr_constructor(godot_object *p_instance);
void GDN_EXPORT godot_arvr_destructor(void *p_data);
godot_string GDN_EXPORT godot_arvr_get_name(void *p_data);
godot_int GDN_EXPORT godot_arvr_get_capabilities(void *p_data);
godot_bool GDN_EXPORT godot_arvr_get_anchor_detection_is_enabled(void *p_data);
void GDN_EXPORT godot_arvr_set_anchor_detection_is_enabled(void *p_data,
		bool p_enable);
godot_bool GDN_EXPORT godot_arvr_is_stereo(void *p_data);
godot_bool GDN_EXPORT godot_arvr_is_initialized(void *p_data);
godot_bool GDN_EXPORT godot_arvr_initialize(void *p_data);
void GDN_EXPORT godot_arvr_uninitialize(void *p_data);
godot_vector2 GDN_EXPORT
godot_arvr_get_recommended_render_targetsize(void *p_data);
godot_transform GDN_EXPORT godot_arvr_get_transform_for_eye(
		void *p_data, godot_int p_eye, godot_transform *p_cam_transform);
void GDN_EXPORT godot_arvr_fill_projection_for_eye(
		void *p_data, godot_real *p_projection, godot_int p_eye,
		godot_real p_aspect, godot_real p_z_near, godot_real p_z_far);
void GDN_EXPORT godot_arvr_commit_for_eye(void *p_data, godot_int p_eye,
		godot_rid *p_render_target,
		godot_rect2 *p_screen_rect);
void GDN_EXPORT godot_arvr_process(void *p_data);

#ifdef __cplusplus
}
#endif

#endif /* !OVR_ARVR_INTERFACE_H */
