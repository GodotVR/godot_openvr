////////////////////////////////////////////////////////////////////////////////////////////////
// Just exposing an interface to the ArrayMesh functions reachable in GDNative
// Basically just ported some of the functions in the cpp_bindings for use in C

#ifndef ARRAY_MESH_H
#define ARRAY_MESH_H

#include "GodotCalls.h"

enum ArrayType {

	ARRAY_VERTEX = 0,
	ARRAY_NORMAL = 1,
	ARRAY_TANGENT = 2,
	ARRAY_COLOR = 3,
	ARRAY_TEX_UV = 4,
	ARRAY_TEX_UV2 = 5,
	ARRAY_BONES = 6,
	ARRAY_WEIGHTS = 7,
	ARRAY_INDEX = 8,
	ARRAY_MAX = 9

};

enum ArrayFormat {
	/* ARRAY FORMAT FLAGS */
	ARRAY_FORMAT_VERTEX = 1 << ARRAY_VERTEX, // mandatory
	ARRAY_FORMAT_NORMAL = 1 << ARRAY_NORMAL,
	ARRAY_FORMAT_TANGENT = 1 << ARRAY_TANGENT,
	ARRAY_FORMAT_COLOR = 1 << ARRAY_COLOR,
	ARRAY_FORMAT_TEX_UV = 1 << ARRAY_TEX_UV,
	ARRAY_FORMAT_TEX_UV2 = 1 << ARRAY_TEX_UV2,
	ARRAY_FORMAT_BONES = 1 << ARRAY_BONES,
	ARRAY_FORMAT_WEIGHTS = 1 << ARRAY_WEIGHTS,
	ARRAY_FORMAT_INDEX = 1 << ARRAY_INDEX,

	ARRAY_COMPRESS_BASE = (ARRAY_INDEX + 1),
	ARRAY_COMPRESS_VERTEX = 1 << (ARRAY_VERTEX + ARRAY_COMPRESS_BASE), // mandatory
	ARRAY_COMPRESS_NORMAL = 1 << (ARRAY_NORMAL + ARRAY_COMPRESS_BASE),
	ARRAY_COMPRESS_TANGENT = 1 << (ARRAY_TANGENT + ARRAY_COMPRESS_BASE),
	ARRAY_COMPRESS_COLOR = 1 << (ARRAY_COLOR + ARRAY_COMPRESS_BASE),
	ARRAY_COMPRESS_TEX_UV = 1 << (ARRAY_TEX_UV + ARRAY_COMPRESS_BASE),
	ARRAY_COMPRESS_TEX_UV2 = 1 << (ARRAY_TEX_UV2 + ARRAY_COMPRESS_BASE),
	ARRAY_COMPRESS_BONES = 1 << (ARRAY_BONES + ARRAY_COMPRESS_BASE),
	ARRAY_COMPRESS_WEIGHTS = 1 << (ARRAY_WEIGHTS + ARRAY_COMPRESS_BASE),
	ARRAY_COMPRESS_INDEX = 1 << (ARRAY_INDEX + ARRAY_COMPRESS_BASE),

	ARRAY_FLAG_USE_2D_VERTICES = ARRAY_COMPRESS_INDEX << 1,
	ARRAY_FLAG_USE_16_BIT_BONES = ARRAY_COMPRESS_INDEX << 2,

	ARRAY_COMPRESS_DEFAULT = ARRAY_COMPRESS_VERTEX | ARRAY_COMPRESS_NORMAL | ARRAY_COMPRESS_TANGENT | ARRAY_COMPRESS_COLOR | ARRAY_COMPRESS_TEX_UV | ARRAY_COMPRESS_TEX_UV2 | ARRAY_COMPRESS_WEIGHTS

};

enum PrimitiveType {
	PRIMITIVE_POINTS = 0,
	PRIMITIVE_LINES = 1,
	PRIMITIVE_LINE_STRIP = 2,
	PRIMITIVE_LINE_LOOP = 3,
	PRIMITIVE_TRIANGLES = 4,
	PRIMITIVE_TRIANGLE_STRIP = 5,
	PRIMITIVE_TRIANGLE_FAN = 6,
	PRIMITIVE_MAX = 7,
};

#ifdef __cplusplus
extern "C" {
#endif

int64_t ArrayMesh_get_surface_count(godot_object *p_this);
void ArrayMesh_surface_remove(godot_object *p_this, const int64_t surf_idx);
void ArrayMesh_add_surface_from_arrays(godot_object *p_this, const int64_t primitive, const godot_array& arrays, const godot_array& blend_shapes, const int64_t compress_flags);
void ArrayMesh_surface_set_material(godot_object *p_this, const int64_t surf_idx, godot_object *material);

#ifdef __cplusplus
}
#endif


#endif