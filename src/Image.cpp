////////////////////////////////////////////////////////////////////////////////////////////////
// Just exposing an interface to the Image functions reachable in GDNative
// Basically just ported some of the functions in the cpp_bindings for use in C

// Note, even though this is pure C code, we're using the C++ compiler as
// Microsoft never updated their C compiler to understand more modern dialects
// and openvr uses pesky things such as namespaces

#include "Image.h"

godot_object *Image_new() {
	return api->godot_get_class_constructor((char *)"Image")();
};

void Image_create_from_data(godot_object *p_this, const int64_t width, const int64_t height, const bool use_mipmaps, const int64_t format, const godot_pool_byte_array *data) {
	static godot_method_bind *mb = NULL;
	if (mb == NULL) {
		mb = api->godot_method_bind_get_method("Image", "create_from_data");
	}
	___godot_icall_void_int_int_bool_int_PoolByteArray(mb, p_this, width, height, use_mipmaps, format, data);
}
