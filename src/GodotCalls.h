////////////////////////////////////////////////////////////////////////////////////////////////
// Just exposing an interface to a few callback functions on Godot objects
// Basically just ported some of the functions in the cpp_bindings for use in C

#ifndef GODOT_CALLS_H
#define GODOT_CALLS_H

#include <gdnative_api_struct.gen.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstdlib>

// forward declarations
extern const godot_gdnative_api_struct *api;

#ifdef __cplusplus
extern "C" {
#endif

int64_t ___godot_icall_int(godot_method_bind *mb, godot_object *inst);
void ___godot_icall_void_int(godot_method_bind *mb, godot_object *inst, const int arg0);
void ___godot_icall_void_int_Array_Array_int(godot_method_bind *mb, godot_object *inst, const int arg0, const godot_array& arg1, const godot_array& arg2, const int arg3);
void ___godot_icall_void_int_Object(godot_method_bind *mb, godot_object *inst, const int arg0, const godot_object *arg1);
void ___godot_icall_void_Color(godot_method_bind *mb, godot_object *inst, const godot_color& arg0);
void ___godot_icall_void_Object_int(godot_method_bind *mb, godot_object *inst, const godot_object *arg0, const int arg1);
void ___godot_icall_void_int_int_bool_int_PoolByteArray(godot_method_bind *mb, godot_object *inst, const int arg0, const int arg1, const bool arg2, const int arg3, const godot_pool_byte_array *arg4);

#ifdef __cplusplus
}
#endif


#endif /* !GODOT_CALLS_H */
