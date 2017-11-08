////////////////////////////////////////////////////////////////////////////////////////////////
// Just exposing an interface to the ImageTexture functions reachable in GDNative
// Basically just ported some of the functions in the cpp_bindings for use in C

#ifndef IMAGE_TEXTURE_H
#define IMAGE_TEXTURE_H

#include "GodotCalls.h"
#include "Image.h"

typedef godot_object godot_image_texture;

#ifdef __cplusplus
extern "C" {
#endif

godot_image_texture *ImageTexture_new();
void ImageTexture_delete(godot_image_texture *ptr);
void ImageTexture_create_from_image(godot_image_texture *p_this, const godot_image *image, const int64_t flags);

#ifdef __cplusplus
}
#endif

#endif /* !IMAGE_TEXTURE_H */
