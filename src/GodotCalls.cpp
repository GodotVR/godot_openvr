////////////////////////////////////////////////////////////////////////////////////////////////
// Just exposing an interface to a few callback functions on Godot objects
// Basically just ported some of the functions in the cpp_bindings for use in C

// Note, even though this is pure C code, we're using the C++ compiler as
// Microsoft never updated their C compiler to understand more modern dialects
// and openvr uses pesky things such as namespaces

#include "GodotCalls.h"

const godot_gdnative_api_struct *api = NULL;
const godot_gdnative_nativearvr_api_struct *arvr_api = NULL;
const godot_gdnative_nativescript_api_struct *nativescript_api = NULL;

int64_t ___godot_icall_int(godot_method_bind *mb, godot_object *inst) {
	int64_t ret;
	const void *args[1] = {
	};

	api->godot_method_bind_ptrcall(mb, inst, args, &ret);
	return ret;
}

void ___godot_icall_void_int(godot_method_bind *mb, godot_object *inst, const int arg0) {
	const void *args[] = {
		(void *) &arg0,
	};

	api->godot_method_bind_ptrcall(mb, inst, args, nullptr);
}

void ___godot_icall_void_int_Array_Array_int(godot_method_bind *mb, godot_object *inst, const int arg0, const godot_array& arg1, const godot_array& arg2, const int arg3) {
	const void *args[] = {
		(void *) &arg0,
		(void *) &arg1,
		(void *) &arg2,
		(void *) &arg3,
	};

	api->godot_method_bind_ptrcall(mb, inst, args, nullptr);
}

void ___godot_icall_void_int_Object(godot_method_bind *mb, godot_object *inst, const int arg0, const godot_object *arg1) {
	const void *args[] = {
		(void *) &arg0,
		(void *) arg1,
	};

	api->godot_method_bind_ptrcall(mb, inst, args, nullptr);
}

void ___godot_icall_void_Color(godot_method_bind *mb, godot_object *inst, const godot_color& arg0) {
	const void *args[] = {
		(void *) &arg0,
	};

	api->godot_method_bind_ptrcall(mb, inst, args, nullptr);
}

void ___godot_icall_void_Object_int(godot_method_bind *mb, godot_object *inst, const godot_object *arg0, const int arg1) {
	const void *args[] = {
		(void *) arg0,
		(void *) &arg1,
	};

	api->godot_method_bind_ptrcall(mb, inst, args, nullptr);
}

void ___godot_icall_void_int_int_bool_int_PoolByteArray(godot_method_bind *mb, godot_object *inst, const int arg0, const int arg1, const bool arg2, const int arg3, const godot_pool_byte_array *arg4) {
	const void *args[] = {
		(void *) &arg0,
		(void *) &arg1,
		(void *) &arg2,
		(void *) &arg3,
		(void *) arg4,
	};

	api->godot_method_bind_ptrcall(mb, inst, args, nullptr);
}
