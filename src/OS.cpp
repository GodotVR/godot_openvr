////////////////////////////////////////////////////////////////////////////////////////////////
// Just exposing an interface to the OS functions reachable in GDNative
// Basically just ported some of the functions in the cpp_bindings for use in C

// Note, even though this is pure C code, we're using the C++ compiler as
// Microsoft never updated their C compiler to understand more modern dialects
// and openvr uses pesky things such as namespaces

#include "OS.h"

static godot_object *___static_object_OS;

static inline void OS_singleton_init() {
	if (___static_object_OS == nullptr) {
		___static_object_OS = api->godot_global_get_singleton((char *) "OS");
	}
}

int64_t OS_get_ticks_msec() {
	OS_singleton_init();
	static godot_method_bind *mb = NULL;
	if (mb == NULL) {
		mb = api->godot_method_bind_get_method("_OS", "get_ticks_msec");
	}
	return ___godot_icall_int(mb, (godot_object *) ___static_object_OS);
}

godot_vector2 OS_get_screen_size(const int64_t screen) {
	OS_singleton_init();
	static godot_method_bind *mb = NULL;
	if (mb == NULL) {
		mb = api->godot_method_bind_get_method("_OS", "get_screen_size");
	}
	return ___godot_icall_Vector2_int(mb, (godot_object *) ___static_object_OS, screen);
}
