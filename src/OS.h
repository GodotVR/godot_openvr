////////////////////////////////////////////////////////////////////////////////////////////////
// Just exposing an interface to the ImageTexture functions reachable in GDNative
// Basically just ported some of the functions in the cpp_bindings for use in C

#ifndef OS_H
#define OS_H

#include "GodotCalls.h"

#ifdef __cplusplus
extern "C" {
#endif

int64_t OS_get_ticks_msec();

#ifdef __cplusplus
}
#endif

#endif /* !OS_H */
