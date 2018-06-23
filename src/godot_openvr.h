////////////////////////////////////////////////////////////////////////////
// OpenVR GDNative module for Godot
//
// Written by Bastiaan "Mux213" Olij, 
// with loads of help from Thomas "Karroffel" Herzog

#ifndef GODOT_OPENVR_H
#define GODOT_OPENVR_H

#include "GodotCalls.h"
#include "OVRCalls.h"
	
#include "ARVRInterface.h"
#include "OVRRenderModel.h"

#ifdef __cplusplus
extern "C" {
#endif

void GDN_EXPORT godot_openvr_gdnative_singleton();
void GDN_EXPORT godot_openvr_nativescript_init(void *p_handle);

#ifdef __cplusplus
}
#endif

#endif /* !GODOT_OPENVR_H */
