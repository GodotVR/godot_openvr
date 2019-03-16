////////////////////////////////////////////////////////////////////////////////////////////////
// GDNative module that exposes some OpenVR module options to Godot

#ifndef OPENVR_CONFIG_H
#define OPENVR_CONFIG_H

#include "GodotCalls.h"
#include "OVRCalls.h"
#include "openvr_config_data.h"

#ifdef __cplusplus
extern "C" {
#endif

GDCALLINGCONV void *openvr_config_constructor(godot_object *p_instance, void *p_method_data);
GDCALLINGCONV void openvr_config_destructor(godot_object *p_instance, void *p_method_data, void *p_user_data);
GDCALLINGCONV godot_variant openvr_config_get_application_type(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);
GDCALLINGCONV godot_variant openvr_config_set_application_type(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);
GDCALLINGCONV godot_variant openvr_config_get_tracking_universe(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);
GDCALLINGCONV godot_variant openvr_config_set_tracking_universe(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);

#ifdef __cplusplus
}
#endif

#endif /* !OPENVR_CONFIG_H */
