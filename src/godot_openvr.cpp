////////////////////////////////////////////////////////////////////////////
// OpenVR GDNative module for Godot
//
// Written by Bastiaan "Mux213" Olij, 
// with loads of help from Thomas "Karroffel" Herzog

// Note, even though this is pure C code, we're using the C++ compiler as
// Microsoft never updated their C compiler to understand more modern dialects
// and openvr uses pesky things such as namespaces

#include "godot_openvr.h"

void GDN_EXPORT godot_openvr_gdnative_singleton() {
	if (arvr_api != NULL) {
		arvr_api->godot_arvr_register_interface(&interface_struct);
	}
}

void GDN_EXPORT godot_openvr_nativescript_init(void *p_handle) {
	if (nativescript_api == NULL) {
		return;
	}

	{
		godot_instance_create_func create = { NULL, NULL, NULL };
		create.create_func = &openvr_render_model_constructor;

		godot_instance_destroy_func destroy = { NULL, NULL, NULL };
		destroy.destroy_func = &openvr_render_model_destructor;

		nativescript_api->godot_nativescript_register_class(p_handle, "OpenVRRenderModel", "ArrayMesh", create, destroy);
	}

	{
		godot_instance_method get_data = { NULL, NULL, NULL };
		get_data.method = &openvr_render_model_list;

		godot_method_attributes attributes = { GODOT_METHOD_RPC_MODE_DISABLED };

		nativescript_api->godot_nativescript_register_method(p_handle, "OpenVRRenderModel", "model_names", attributes, get_data);
	}

	{
		godot_instance_method get_data = { NULL, NULL, NULL };
		get_data.method = &openvr_render_model_load;

		godot_method_attributes attributes = { GODOT_METHOD_RPC_MODE_DISABLED };

		nativescript_api->godot_nativescript_register_method(p_handle, "OpenVRRenderModel", "load_model", attributes, get_data);
	}
}
