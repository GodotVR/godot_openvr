////////////////////////////////////////////////////////////////////////////////////////////////
// Just exposing an interface to the Visual Server functions reachable in GDNative
// Basically just ported some of the functions in the cpp_bindings for use in C

#include "VisualServer.h"

VisualServer *VisualServer::_singleton = NULL;

godot_rid VisualServer::texture_create() {
	static godot_method_bind *mb = NULL;
	if (mb == NULL) {
		mb = api->godot_method_bind_get_method("VisualServer", "texture_create");
	}
	return ___godot_icall_RID(mb, (godot_object *)_owner);
}

godot_rid VisualServer::texture_create_from_image(godot_object *image, const int64_t flags) {
	static godot_method_bind *mb = NULL;
	if (mb == NULL) {
		mb = api->godot_method_bind_get_method("VisualServer", "texture_create_from_image");
	}

	return ___godot_icall_RID_Object_int(mb, (godot_object *)_owner, image, flags);
}

void VisualServer::texture_allocate(const godot_rid texture, const int64_t width, const int64_t height, const int64_t depth_3d, const int64_t format, const int64_t type, const int64_t flags) {
	static godot_method_bind *mb = NULL;
	if (mb == NULL) {
		mb = api->godot_method_bind_get_method("VisualServer", "texture_allocate");
	}

	___godot_icall_void_RID_int_int_int_int_int_int(mb, (godot_object *)_owner, texture, width, height, depth_3d, format, type, flags);
}

int64_t VisualServer::texture_get_texid(const godot_rid texture) const {
	static godot_method_bind *mb = NULL;
	if (mb == NULL) {
		mb = api->godot_method_bind_get_method("VisualServer", "texture_get_texid");
	}

	return ___godot_icall_int_RID(mb, (godot_object *)_owner, texture);
}

void VisualServer::free_rid(const godot_rid rid) {
	static godot_method_bind *mb = NULL;
	if (mb == NULL) {
		mb = api->godot_method_bind_get_method("VisualServer", "free_rid");
	}
	___godot_icall_void_RID(mb, (godot_object *)_owner, rid);
}

VisualServer::VisualServer() {
	_owner = api->godot_global_get_singleton((char *)"VisualServer");
}