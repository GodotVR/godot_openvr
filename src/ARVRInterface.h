////////////////////////////////////////////////////////////////////////////////////////////////
// Our main ARVRInterface code for our OpenVR GDNative module

#ifndef OVR_ARVR_INTERFACE_H
#define OVR_ARVR_INTERFACE_H

#include "godot_openvr.h"
#include "openvr_data.h"

extern const godot_arvr_interface_gdnative interface_struct;

typedef struct arvr_data_struct {
	openvr_data *ovr;
	uint32_t width;
	uint32_t height;

	vr::TrackedDevicePose_t tracked_device_pose[vr::k_unMaxTrackedDeviceCount];
	vr::VRControllerState_t tracked_device_state[vr::k_unMaxTrackedDeviceCount];
	godot_int trackers[vr::k_unMaxTrackedDeviceCount];
	uint64_t last_rumble_update[vr::k_unMaxTrackedDeviceCount];
	godot_transform hmd_transform;
	bool device_hands_are_available;
	uint32_t left_hand_device;
	uint32_t right_hand_device;

	int video_driver;
	int texture_id;
} arvr_data_struct;

#endif /* !OVR_ARVR_INTERFACE_H */
