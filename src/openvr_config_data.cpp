////////////////////////////////////////////////////////////////////////////////////////////////
// Shared data structure for our OpenVR GDNative module

#include "openvr_config_data.h"

// We should only have one instance of our ARVR Interface registered with our server, we'll keep a single reference to our data structure
// Note that we should have our interface, even when not needed (yet), loaded automatically at startup, so basically this structure will
// be accessible from the get go.
openvr_config_data_struct *openvr_config_data = NULL;

openvr_config_data_struct *get_openvr_config_data() {
	if (openvr_config_data == NULL) {
		openvr_config_data = (openvr_config_data_struct *)api->godot_alloc(sizeof(openvr_config_data_struct));
		openvr_config_data->use_count = 1;
		openvr_config_data->application_type = OpenVRApplicationType::SCENE;
		openvr_config_data->tracking_universe = OpenVRTrackingUniverse::STANDING;
	} else {
		openvr_config_data->use_count++;
	}

	return openvr_config_data;
};

void release_openvr_config_data() {
	if (openvr_config_data != NULL) {
		if (openvr_config_data->use_count > 1) {
			openvr_config_data->use_count--;
		} else if (openvr_config_data->use_count == 1) {
			api->godot_free(openvr_config_data);
			openvr_config_data = NULL;
		}
	}
};

int openvr_config_get_application_type() {
	if (openvr_config_data == NULL) {
		// Not yet initialised!
		return 1;
	} else {
		if (openvr_config_data->application_type == OpenVRApplicationType::OVERLAY) {
			return 2;
		} else {
			return 1;
		}
	}
}

void openvr_config_set_application_type(int p_new_value) {
	if (openvr_config_data == NULL) {
		// Not yet initialised!
	} else {
		if (p_new_value == 2) {
			openvr_config_data->application_type = OpenVRApplicationType::OVERLAY;
		} else {
			openvr_config_data->application_type = OpenVRApplicationType::SCENE;		
		}
	}
}

int openvr_config_get_tracking_universe() {
	if (openvr_config_data == NULL) {
		// Not yet initialised!
		return 1;
	} else {
		if (openvr_config_data->tracking_universe == OpenVRTrackingUniverse::SEATED) {
			return 0;
		} else if (openvr_config_data->tracking_universe == OpenVRTrackingUniverse::STANDING) {
			return 1;
		} else {
			return 2;
		}
	}
}

void openvr_config_set_tracking_universe(int p_new_value) {
	if (openvr_config_data == NULL) {
		// Not yet initialised!
	} else {
		if (p_new_value == 0) {
			openvr_config_data->tracking_universe = OpenVRTrackingUniverse::SEATED;
		} else if (p_new_value == 1) {
			openvr_config_data->tracking_universe = OpenVRTrackingUniverse::STANDING;
		} else {
			openvr_config_data->tracking_universe = OpenVRTrackingUniverse::RAW;		
		}
	}
}
