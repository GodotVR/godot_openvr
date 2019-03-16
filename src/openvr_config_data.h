////////////////////////////////////////////////////////////////////////////////////////////////
// Shared data structure for our OpenVR GDNative module

#ifndef OPENVR_CONFIG_DATA_H
#define OPENVR_CONFIG_DATA_H

#include "GodotCalls.h"

enum OpenVRApplicationType {OTHER, SCENE, OVERLAY};
enum OpenVRTrackingUniverse {SEATED, STANDING, RAW};

typedef struct openvr_config_data_struct {
	int use_count; // should always be 1!
    enum OpenVRApplicationType application_type;
    enum OpenVRTrackingUniverse tracking_universe;
} openvr_config_data_struct;

openvr_config_data_struct *get_openvr_config_data();
void release_openvr_config_data();
int openvr_config_get_application_type();
void openvr_config_set_application_type(int p_new_value);
int openvr_config_get_tracking_universe();
void openvr_config_set_tracking_universe(int p_new_value);

#endif /* !OPENVR_CONFIG_DATA_H */
