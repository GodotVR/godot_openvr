////////////////////////////////////////////////////////////////////////////////////////////////
// Helper calls and singleton container for accessing openvr

#ifndef OVR_CALLS_H
#define OVR_CALLS_H

#include "GodotCalls.h"
#include <openvr.h>

typedef struct openvr_data_struct {
	int use_count;
	vr::IVRSystem *hmd;
	vr::IVRRenderModels *render_models;
} openvr_data_struct;

#ifdef __cplusplus
extern "C" {
#endif

void ThreadSleep( unsigned long nMilliseconds );
void openvr_release_data();
openvr_data_struct *openvr_get_data();
char *openvr_get_device_name(openvr_data_struct *p_openvr_data, vr::TrackedDeviceIndex_t p_tracked_device_index, int pMaxLen);
void openvr_transform_from_matrix(godot_transform *p_dest, vr::HmdMatrix34_t *p_matrix, godot_real p_world_scale);

#ifdef __cplusplus
}
#endif

#endif /* !OVR_CALLS_H */
