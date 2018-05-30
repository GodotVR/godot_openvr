////////////////////////////////////////////////////////////////////////////////////////////////
// Just exposing an interface to a few callback functions on Godot objects
// Basically just ported some of the functions in the cpp_bindings for use in C

#ifndef GODOT_CALLS_H
#define GODOT_CALLS_H

#include <stdint.h>

// fully define these, don't waste time with needless callbacks for access
#define GODOT_CORE_API_GODOT_VECTOR2_TYPE_DEFINED
typedef union {
	//Force struct to be classified as INTEGER by System V AMD64 ABI.
	uint8_t _dont_touch_that[8];
	struct {
		float x;
		float y;
	};

	inline void set(float p_x, float p_y) {
		x = p_x;
		y = p_y;
	};
} godot_vector2;

#define GODOT_CORE_API_GODOT_VECTOR3_TYPE_DEFINED
typedef union {
	//Force struct to be classified as INTEGER by System V AMD64 ABI.
	uint8_t _dont_touch_that[12];
	struct {
		float x;
		float y;
		float z;
	};

	inline void set(float p_x, float p_y, float p_z) {
		x = p_x;
		y = p_y;
		z = p_z;
	};
} godot_vector3;

#define GODOT_CORE_API_GODOT_RECT2_TYPE_DEFINED
typedef union {
	//Force struct to be classified as INTEGER by System V AMD64 ABI.
	uint8_t _dont_touch_that[16];
	struct {
		godot_vector2 position;
		godot_vector2 size;
	};
} godot_rect2;

#include <gdnative_api_struct.gen.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstdlib>

// forward declarations
extern const godot_gdnative_core_api_struct *api;
extern const godot_gdnative_ext_arvr_api_struct *arvr_api;
extern const godot_gdnative_ext_nativescript_api_struct *nativescript_api;

#ifdef __cplusplus
extern "C" {
#endif

void GDN_EXPORT godot_openvr_gdnative_init(godot_gdnative_init_options *p_options);
void GDN_EXPORT godot_openvr_gdnative_terminate(godot_gdnative_terminate_options *p_options);

int64_t ___godot_icall_int(godot_method_bind *mb, godot_object *inst);
void ___godot_icall_void_int(godot_method_bind *mb, godot_object *inst, const int arg0);
void ___godot_icall_void_int_Array_Array_int(godot_method_bind *mb, godot_object *inst, const int arg0, const godot_array& arg1, const godot_array& arg2, const int arg3);
void ___godot_icall_void_int_Object(godot_method_bind *mb, godot_object *inst, const int arg0, const godot_object *arg1);
void ___godot_icall_void_Color(godot_method_bind *mb, godot_object *inst, const godot_color& arg0);
void ___godot_icall_void_Object_int(godot_method_bind *mb, godot_object *inst, const godot_object *arg0, const int arg1);
void ___godot_icall_void_int_int_bool_int_PoolByteArray(godot_method_bind *mb, godot_object *inst, const int arg0, const int arg1, const bool arg2, const int arg3, const godot_pool_byte_array *arg4);
godot_vector2 ___godot_icall_Vector2_int(godot_method_bind *mb, godot_object *inst, const int arg0);

#ifdef __cplusplus
}
#endif

#endif /* !GODOT_CALLS_H */
