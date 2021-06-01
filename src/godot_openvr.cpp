////////////////////////////////////////////////////////////////////////////
// OpenVR GDNative module for Godot
//
// Written by Bastiaan "Mux213" Olij,
// with loads of help from Thomas "Karroffel" Herzog

// Note, even though this is pure C code, we're using the C++ compiler as
// Microsoft never updated their C compiler to understand more modern dialects
// and openvr uses pesky things such as namespaces

#include "godot_openvr.h"
#include <String.hpp>

void GDN_EXPORT godot_openvr_gdnative_init(godot_gdnative_init_options *o) {
	godot::Godot::gdnative_init(o);
}

void GDN_EXPORT godot_openvr_gdnative_terminate(godot_gdnative_terminate_options *o) {
	godot::Godot::gdnative_terminate(o);
}

void GDN_EXPORT godot_openvr_gdnative_singleton() {
	if (godot::xr_api != NULL) {
		godot::xr_api->godot_xr_register_interface(&interface_struct);
	}
}

void GDN_EXPORT godot_openvr_nativescript_init(void *p_handle) {
	godot::Godot::nativescript_init(p_handle);

	godot::register_class<godot::OpenVRRenderModel>();
	godot::register_class<godot::OpenVRConfig>();
	godot::register_class<godot::OpenVROverlay>();
	godot::register_class<godot::OpenVRPose>();
	godot::register_class<godot::OpenVRAction>();
	godot::register_class<godot::OpenVRHaptics>();
	godot::register_class<godot::OpenVRController>();
	godot::register_class<godot::OpenVRSkeleton>();
}
