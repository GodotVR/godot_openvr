////////////////////////////////////////////////////////////////////////////////////////////////
// Just exposing an interface to the Material functions reachable in GDNative
// Basically just ported some of the functions in the cpp_bindings for use in C

// Note, even though this is pure C code, we're using the C++ compiler as
// Microsoft never updated their C compiler to understand more modern dialects
// and openvr uses pesky things such as namespaces

#include "SpatialMaterial.h"

godot_object *SpatialMaterial_new() {
	return api->godot_get_class_constructor((char *)"SpatialMaterial")();
};

void SpatialMaterial_set_albedo(godot_object *p_this, const godot_color albedo) {
	static godot_method_bind *mb = NULL;
	if (mb == NULL) {
		mb = api->godot_method_bind_get_method("SpatialMaterial", "set_albedo");
	}
	___godot_icall_void_Color(mb, p_this, albedo);
}

void SpatialMaterial_set_texture(godot_object *p_this, const int64_t param, const godot_object *texture) {
	static godot_method_bind *mb = NULL;
	if (mb == NULL) {
		mb = api->godot_method_bind_get_method("SpatialMaterial", "set_texture");
	}
	___godot_icall_void_int_Object(mb, p_this, param, texture);
}

