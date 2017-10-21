////////////////////////////////////////////////////////////////////////////////////////////////
// Just exposing an interface to the ArrayMesh functions reachable in GDNative
// Basically just ported some of the functions in the cpp_bindings for use in C

// Note, even though this is pure C code, we're using the C++ compiler as
// Microsoft never updated their C compiler to understand more modern dialects
// and openvr uses pesky things such as namespaces

#include "ArrayMesh.h"

int64_t ArrayMesh_get_surface_count(godot_object *p_this) {
	static godot_method_bind *mb = NULL;
	if (mb == NULL) {
		mb = api->godot_method_bind_get_method("ArrayMesh", "get_surface_count");
	}
	return ___godot_icall_int(mb, p_this);
}

void ArrayMesh_surface_remove(godot_object *p_this, const int64_t surf_idx) {
	static godot_method_bind *mb = NULL;
	if (mb == NULL) {
		mb = api->godot_method_bind_get_method("ArrayMesh", "surface_remove");
	}
	___godot_icall_void_int(mb, p_this, surf_idx);
}

void ArrayMesh_add_surface_from_arrays(godot_object *p_this, const int64_t primitive, const godot_array& arrays, const godot_array& blend_shapes, const int64_t compress_flags) {
	static godot_method_bind *mb = NULL;
	if (mb == NULL) {
		mb = api->godot_method_bind_get_method("ArrayMesh", "add_surface_from_arrays");
	}
	___godot_icall_void_int_Array_Array_int(mb, p_this, primitive, arrays, blend_shapes, compress_flags);
}

void ArrayMesh_surface_set_material(godot_object *p_this, const int64_t surf_idx, godot_object *material) {
	static godot_method_bind *mb = NULL;
	if (mb == NULL) {
		mb = api->godot_method_bind_get_method("ArrayMesh", "surface_set_material");
	}
	___godot_icall_void_int_Object(mb, p_this, surf_idx, material);
}
