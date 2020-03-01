////////////////////////////////////////////////////////////////////////////
// OpenVR GDNative module for Godot
//
// Written by Bastiaan "Mux213" Olij,
// with loads of help from Thomas "Karroffel" Herzog

#ifndef GODOT_OPENVR_H
#define GODOT_OPENVR_H

#include "openvr_data.h"

#include "ARVRInterface.h"
#include "OpenVRAction.h"
#include "OpenVRConfig.h"
#include "OpenVRController.h"
#include "OpenVRHaptics.h"
#include "OpenVROverlay.h"
#include "OpenVRPose.h"
#include "OpenVRRenderModel.h"

#ifdef __cplusplus
extern "C" {
#endif

void GDN_EXPORT godot_openvr_gdnative_init(godot_gdnative_init_options *o);
void GDN_EXPORT godot_openvr_gdnative_terminate(godot_gdnative_terminate_options *o);
void GDN_EXPORT godot_openvr_gdnative_singleton();
void GDN_EXPORT godot_openvr_nativescript_init(void *p_handle);

#ifdef __cplusplus
}
#endif

#endif /* !GODOT_OPENVR_H */
