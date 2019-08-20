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

const godot_gdnative_ext_arvr_api_struct *arvr_api = NULL;

void GDN_EXPORT godot_openvr_gdnative_init(godot_gdnative_init_options *o) {
	godot::Godot::gdnative_init(o);

	// this should move into godot-cpp
	for (int i = 0; i < godot::api->num_extensions; i++) {
		// todo: add version checks
		switch (godot::api->extensions[i]->type) {
			case GDNATIVE_EXT_ARVR: {
				if (godot::api->extensions[i]->version.major > 1 || (godot::api->extensions[i]->version.major == 1 && godot::api->extensions[i]->version.minor >= 1)) {
					arvr_api = (godot_gdnative_ext_arvr_api_struct *)godot::api->extensions[i];
				} else {
					godot::Godot::print(
							godot::String("ARVR API version ") + godot::String::num_int64(godot::api->extensions[i]->version.major) + godot::String(".") + godot::String::num_int64(godot::api->extensions[i]->version.minor) + godot::String(" isn't supported, need version 1.1 or higher"));
				}
			}; break;
			default: break;
		}
	}
}

void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options *o) {
	arvr_api = NULL;

	godot::Godot::gdnative_terminate(o);
}

void GDN_EXPORT godot_openvr_gdnative_singleton() {
	if (arvr_api != NULL) {
		arvr_api->godot_arvr_register_interface(&interface_struct);
	}
}

void GDN_EXPORT godot_openvr_nativescript_init(void *p_handle) {
	godot::Godot::nativescript_init(p_handle);

	godot::register_class<godot::OpenVRRenderModel>();
	godot::register_class<godot::OpenVRConfig>();
	godot::register_class<godot::OpenVROverlay>();

	/*

	{
		godot_instance_method get_data = { NULL, NULL, NULL };
		get_data.method = &openvr_overlay_position_absolute;

		godot_method_attributes attributes = { GODOT_METHOD_RPC_MODE_DISABLED };

		nativescript_api->godot_nativescript_register_method(p_handle, "OpenVROverlay", "overlay_position_absolute", attributes, get_data);
	}
*/
}
