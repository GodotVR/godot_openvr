////////////////////////////////////////////////////////////////////////////////////////////////
// Just exposing an interface to the Image functions reachable in GDNative
// Basically just ported some of the functions in the cpp_bindings for use in C

#ifndef IMAGE_H
#define IMAGE_H

#include "GodotCalls.h"

enum Format {
	FORMAT_RGTC_RG = 21,
	FORMAT_RGB8 = 4,
	FORMAT_ETC2_RGBA8 = 35,
	FORMAT_BPTC_RGBFU = 24,
	FORMAT_MAX = 37,
	FORMAT_RG8 = 3,
	FORMAT_BPTC_RGBA = 22,
	FORMAT_RGBE9995 = 16,
	FORMAT_ETC2_RGB8A1 = 36,
	FORMAT_ETC2_R11S = 31,
	FORMAT_R8 = 2,
	FORMAT_RGBAH = 15,
	FORMAT_RGBAF = 11,
	FORMAT_ETC = 29,
	FORMAT_RF = 8,
	FORMAT_ETC2_R11 = 30,
	FORMAT_ETC2_RGB8 = 34,
	FORMAT_RGBA5551 = 7,
	FORMAT_RH = 12,
	FORMAT_RGTC_R = 20,
	FORMAT_RGBA8 = 5,
	FORMAT_L8 = 0,
	FORMAT_ETC2_RG11 = 32,
	FORMAT_RGBF = 10,
	FORMAT_ETC2_RG11S = 33,
	FORMAT_LA8 = 1,
	FORMAT_PVRTC4 = 27,
	FORMAT_RGF = 9,
	FORMAT_RGBH = 14,
	FORMAT_PVRTC2A = 26,
	FORMAT_PVRTC2 = 25,
	FORMAT_PVRTC4A = 28,
	FORMAT_RGBA4444 = 6,
	FORMAT_DXT5 = 19,
	FORMAT_DXT3 = 18,
	FORMAT_RGH = 13,
	FORMAT_DXT1 = 17,
	FORMAT_BPTC_RGBF = 23,
};


#ifdef __cplusplus
extern "C" {
#endif

godot_object *Image_new();
void Image_create_from_data(godot_object *p_this, const int64_t width, const int64_t height, const bool use_mipmaps, const int64_t format, const godot_pool_byte_array *data);

#ifdef __cplusplus
}
#endif

#endif /* !IMAGE_H */
