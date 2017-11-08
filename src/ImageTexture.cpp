////////////////////////////////////////////////////////////////////////////////////////////////
// Just exposing an interface to the ImageTexture functions reachable in GDNative
// Basically just ported some of the functions in the cpp_bindings for use in C

// Note, even though this is pure C code, we're using the C++ compiler as
// Microsoft never updated their C compiler to understand more modern dialects
// and openvr uses pesky things such as namespaces

#include "ImageTexture.h"

godot_image_texture *ImageTexture_new() {
	return (godot_image_texture *)api->godot_get_class_constructor((char *)"ImageTexture")();
};

void ImageTexture_delete(godot_image_texture *ptr) {
	api->godot_object_destroy(ptr);
};

void ImageTexture_create_from_image(godot_image_texture *p_this, const godot_image *image, const int64_t flags) {
	static godot_method_bind *mb = NULL;
	if (mb == NULL) {
		mb = api->godot_method_bind_get_method("ImageTexture", "create_from_image");
	}
	___godot_icall_void_Object_int(mb, p_this, image, flags);
}
