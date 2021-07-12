////////////////////////////////////////////////////////////////////////////////////////////////
// Our main XRInterface code for our OpenVR GDNative module

#ifndef OVR_ARVR_INTERFACE_H
#define OVR_ARVR_INTERFACE_H

#include "godot_openvr.h"
#include "openvr_data.h"

extern const godot_xr_interface_gdnative interface_struct;

typedef struct arvr_data_struct {
	openvr_data *ovr;
	uint32_t width;
	uint32_t height;

	godot::OS::VideoDriver video_driver;
	int texture_id;
} arvr_data_struct;

#endif /* !OVR_ARVR_INTERFACE_H */
