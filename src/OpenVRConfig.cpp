////////////////////////////////////////////////////////////////////////////////////////////////
// GDNative module that exposes some OpenVR module options to Godot

// Note, even though this is pure C code, we're using the C++ compiler as
// Microsoft never updated their C compiler to understand more modern dialects
// and openvr uses pesky things such as namespaces

#include "OpenVRConfig.h"

GDCALLINGCONV void *openvr_config_constructor(godot_object *p_instance, void *p_method_data) {
	return get_openvr_config_data();
}

GDCALLINGCONV void openvr_config_destructor(godot_object *p_instance, void *p_method_data, void *p_user_data) {
	if (p_user_data == NULL) {
		// this should never ever ever ever happen, just being paranoid....
	} else {
		release_openvr_config_data();
	}
}

GDCALLINGCONV godot_variant openvr_config_get_application_type(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {
	godot_variant ret;

	if (p_user_data == NULL) {
		// this should never ever ever ever happen, just being paranoid....
	} else {
		api->godot_variant_new_int(&ret, openvr_config_get_application_type());
	}

	return ret;
}

GDCALLINGCONV godot_variant openvr_config_set_application_type(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {
	godot_variant ret;
	
	if (p_user_data == NULL) {
		// this should never ever ever ever happen, just being paranoid....
	} else {
		openvr_config_set_application_type(api->godot_variant_as_int(p_args[0]));
	}

	api->godot_variant_new_nil(&ret);
	return ret;
}

GDCALLINGCONV godot_variant openvr_config_get_tracking_universe(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {
	godot_variant ret;

	if (p_user_data == NULL) {
		// this should never ever ever ever happen, just being paranoid....
	} else {
		api->godot_variant_new_int(&ret, openvr_config_get_tracking_universe());
	}

	return ret;
}

GDCALLINGCONV godot_variant openvr_config_set_tracking_universe(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {
	godot_variant ret;
	
	if (p_user_data == NULL) {
		// this should never ever ever ever happen, just being paranoid....
	} else {
		openvr_config_set_tracking_universe(api->godot_variant_as_int(p_args[0]));
	}

	api->godot_variant_new_nil(&ret);
	return ret;
}
