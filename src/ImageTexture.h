////////////////////////////////////////////////////////////////////////////////////////////////
// Just exposing an interface to the ImageTexture functions reachable in GDNative
// Basically just ported some of the functions in the cpp_bindings for use in C

#ifndef IMAGE_TEXTURE_H
#define IMAGE_TEXTURE_H

#include "GodotCalls.h"

#ifdef __cplusplus
extern "C" {
#endif

godot_object *ImageTexture_new();
void ImageTexture_create_from_image(godot_object *p_this, const godot_object *image, const int64_t flags);

#ifdef __cplusplus
}
#endif

#endif /* !IMAGE_TEXTURE_H */
