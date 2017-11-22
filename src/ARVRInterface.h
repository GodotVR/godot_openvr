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

void GDN_EXPORT godot_openvr_gdnative_init(godot_gdnative_init_options *p_options);
void GDN_EXPORT godot_openvr_gdnative_terminate(godot_gdnative_terminate_options *p_options);
void GDN_EXPORT godot_openvr_gdnative_singleton();
void GDN_EXPORT godot_openvr_nativescript_init(void *p_handle);

#ifdef __cplusplus
}
#endif

#endif /* !OVR_ARVR_INTERFACE_H */
