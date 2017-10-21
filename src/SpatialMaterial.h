////////////////////////////////////////////////////////////////////////////////////////////////
// Just exposing an interface to the Material functions reachable in GDNative
// Basically just ported some of the functions in the cpp_bindings for use in C

#ifndef SPATIAL_MATERIAL_H
#define SPATIAL_MATERIAL_H

#include "GodotCalls.h"

enum TextureParam {
	TEXTURE_DETAIL_ALBEDO = 14,
	TEXTURE_ALBEDO = 0,
	TEXTURE_METALLIC = 1,
	TEXTURE_TRANSMISSION = 11,
	TEXTURE_DEPTH = 9,
	TEXTURE_FLOWMAP = 7,
	TEXTURE_SUBSURFACE_SCATTERING = 10,
	TEXTURE_EMISSION = 3,
	TEXTURE_ROUGHNESS = 2,
	TEXTURE_NORMAL = 4,
	TEXTURE_CLEARCOAT = 6,
	TEXTURE_REFRACTION = 12,
	TEXTURE_AMBIENT_OCCLUSION = 8,
	TEXTURE_MAX = 16,
	TEXTURE_DETAIL_NORMAL = 15,
	TEXTURE_DETAIL_MASK = 13,
	TEXTURE_RIM = 5,
};

#ifdef __cplusplus
extern "C" {
#endif

godot_object *SpatialMaterial_new();
void SpatialMaterial_set_albedo(godot_object *p_this, const godot_color albedo);
void SpatialMaterial_set_texture(godot_object *p_this, const int64_t param, const godot_object *texture);

#ifdef __cplusplus
}
#endif

#endif /* !SPATIAL_MATERIAL_H */
