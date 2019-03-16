////////////////////////////////////////////////////////////////////////////////////////////////
// GDNative module that exposes overlay functions from OpenVR to Godot

// Note, even though this is pure C code, we're using the C++ compiler as
// Microsoft never updated their C compiler to understand more modern dialects
// and openvr uses pesky things such as namespaces

#include "OpenVROverlay.h"

typedef struct openvr_overlay_data_struct {
	openvr_data_struct *ovr;
} openvr_overlay_data_struct;

GDCALLINGCONV void *openvr_overlay_constructor(godot_object *p_instance, void *p_method_data) {
    openvr_overlay_data_struct *openvr_overlay_data;
	openvr_overlay_data = (openvr_overlay_data_struct *)api->godot_alloc(sizeof(openvr_overlay_data_struct));

	if (openvr_overlay_data != NULL) {
		openvr_overlay_data->ovr = openvr_get_data();
	}

	return openvr_overlay_data;
}

GDCALLINGCONV void openvr_overlay_destructor(godot_object *p_instance, void *p_method_data, void *p_user_data) {
	if (p_user_data != NULL) {
		openvr_overlay_data_struct *openvr_overlay_data = (openvr_overlay_data_struct *) p_user_data;
		if (openvr_overlay_data->ovr != NULL) {
			openvr_release_data();
			openvr_overlay_data->ovr = NULL;
		}
	}
}

GDCALLINGCONV godot_variant openvr_overlay_create_overlay(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {
	godot_variant ret;

    if (p_user_data != NULL && p_num_args > 1) {
		openvr_overlay_data_struct *openvr_overlay_data = (openvr_overlay_data_struct *) p_user_data;
		if (openvr_overlay_data->ovr != NULL) {

            godot_string overlay_key = api->godot_variant_as_string(p_args[0]);
            godot_string overlay_name = api->godot_variant_as_string(p_args[1]);

            godot_char_string overlay_key_cs = api->godot_string_ascii(&overlay_key);
            godot_char_string overlay_name_cs = api->godot_string_ascii(&overlay_name);

            vr::EVROverlayError vrerr = vr::VROverlay()->CreateOverlay(api->godot_char_string_get_data(&overlay_key_cs), api->godot_char_string_get_data(&overlay_name_cs), &openvr_overlay_data->ovr->overlay);
        
			if (vrerr != vr::VROverlayError_None) {
				printf("Could not create overlay, OpenVR error: %i, %s\n", vrerr, vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr));

				api->godot_variant_new_bool(&ret, false);
				return ret;
			}

			api->godot_string_destroy(&overlay_key);
			api->godot_char_string_destroy(&overlay_key_cs);
			api->godot_string_destroy(&overlay_name);
			api->godot_char_string_destroy(&overlay_name_cs);
		}
    }

	api->godot_variant_new_bool(&ret, true);
	return ret;
}

GDCALLINGCONV godot_variant openvr_overlay_destroy_overlay(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {
	godot_variant ret;

    if (p_user_data != NULL) {
		openvr_overlay_data_struct *openvr_overlay_data = (openvr_overlay_data_struct *) p_user_data;
		if (openvr_overlay_data->ovr != NULL) {
            vr::EVROverlayError vrerr = vr::VROverlay()->DestroyOverlay(openvr_overlay_data->ovr->overlay);

			if (vrerr != vr::VROverlayError_None) {
				printf("Could not destroy overlay, OpenVR error: %i, %s\n", vrerr, vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr));

				api->godot_variant_new_bool(&ret, false);
				return ret;
			}
		}
    }

	api->godot_variant_new_bool(&ret, true);
	return ret;
}

GDCALLINGCONV godot_variant openvr_overlay_get_overlay_width_in_meters(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {
	godot_variant ret;
	float overlay_size;

    if (p_user_data != NULL) {
		openvr_overlay_data_struct *openvr_overlay_data = (openvr_overlay_data_struct *) p_user_data;
		if (openvr_overlay_data->ovr != NULL) {
		    vr::VROverlay()->GetOverlayWidthInMeters(openvr_overlay_data->ovr->overlay, &overlay_size);
			api->godot_variant_new_real(&ret, overlay_size);
        }
    }

	return ret;
}

GDCALLINGCONV godot_variant openvr_overlay_set_overlay_width_in_meters(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {
	godot_variant ret;

    if (p_user_data != NULL && p_num_args > 0) {
		openvr_overlay_data_struct *openvr_overlay_data = (openvr_overlay_data_struct *) p_user_data;
		if (openvr_overlay_data->ovr != NULL) {
		    vr::EVROverlayError vrerr = vr::VROverlay()->SetOverlayWidthInMeters(openvr_overlay_data->ovr->overlay, api->godot_variant_as_real(p_args[0]));

			if (vrerr != vr::VROverlayError_None) {
				printf("Could not set overlay width in meters, OpenVR error: %i, %s\n", vrerr, vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr));

				api->godot_variant_new_bool(&ret, false);
				return ret;
			}
        }
    }

	api->godot_variant_new_bool(&ret, true);
	return ret;
}

GDCALLINGCONV godot_variant openvr_overlay_is_overlay_visible(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {
	godot_variant ret;

    if (p_user_data != NULL) {
		openvr_overlay_data_struct *openvr_overlay_data = (openvr_overlay_data_struct *) p_user_data;
		if (openvr_overlay_data->ovr != NULL) {
            bool visible = vr::VROverlay()->IsOverlayVisible(openvr_overlay_data->ovr->overlay);
			api->godot_variant_new_bool(&ret, visible);
		}
    }

	return ret;
}

GDCALLINGCONV godot_variant openvr_overlay_show_overlay(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {
	godot_variant ret;

    if (p_user_data != NULL) {
		openvr_overlay_data_struct *openvr_overlay_data = (openvr_overlay_data_struct *) p_user_data;
		if (openvr_overlay_data->ovr != NULL) {
		    vr::EVROverlayError vrerr = vr::VROverlay()->ShowOverlay(openvr_overlay_data->ovr->overlay);			

			if (vrerr != vr::VROverlayError_None) {
				printf("Could not show overlay, OpenVR error: %i, %s\n", vrerr, vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr));

				api->godot_variant_new_bool(&ret, false);
				return ret;
			}
        }
    }

	api->godot_variant_new_bool(&ret, true);
	return ret;
}

GDCALLINGCONV godot_variant openvr_overlay_hide_overlay(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {
	godot_variant ret;

    if (p_user_data != NULL) {
		openvr_overlay_data_struct *openvr_overlay_data = (openvr_overlay_data_struct *) p_user_data;
		if (openvr_overlay_data->ovr != NULL) {
		    vr::EVROverlayError vrerr = vr::VROverlay()->HideOverlay(openvr_overlay_data->ovr->overlay);

			if (vrerr != vr::VROverlayError_None) {
				printf("Could not hide overlay, OpenVR error: %i, %s\n", vrerr, vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr));

				api->godot_variant_new_bool(&ret, false);
				return ret;
			}
        }
    }

	api->godot_variant_new_bool(&ret, true);
	return ret;
}

GDCALLINGCONV godot_variant openvr_overlay_track_relative_to_device(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {
	godot_variant ret;

    if (p_user_data != NULL && p_num_args > 1) {
		openvr_overlay_data_struct *openvr_overlay_data = (openvr_overlay_data_struct *) p_user_data;
		if (openvr_overlay_data->ovr != NULL) {
			vr::TrackedDeviceIndex_t device = api->godot_variant_as_int(p_args[0]);
			vr::HmdMatrix34_t matrix;
			
			godot_transform gmatrix = api->godot_variant_as_transform(p_args[1]);
			openvr_matrix_from_transform(&matrix, &gmatrix, arvr_api->godot_arvr_get_worldscale());

			vr::EVROverlayError vrerr =  vr::VROverlay()->SetOverlayTransformTrackedDeviceRelative(openvr_overlay_data->ovr->overlay, device, &matrix);

			if (vrerr != vr::VROverlayError_None) {
				printf("Could not track overlay relative to device, OpenVR error: %i, %s\n", vrerr, vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr));

				api->godot_variant_new_bool(&ret, false);
				return ret;
			}
        }
    }

	api->godot_variant_new_bool(&ret, true);
	return ret;
}

GDCALLINGCONV godot_variant openvr_overlay_position_absolute(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args) {
	godot_variant ret;

    if (p_user_data != NULL && p_num_args > 0) {
		openvr_overlay_data_struct *openvr_overlay_data = (openvr_overlay_data_struct *) p_user_data;
		if (openvr_overlay_data->ovr != NULL) {
			vr::HmdMatrix34_t matrix;
			vr::TrackingUniverseOrigin origin;

			godot_transform gmatrix = api->godot_variant_as_transform(p_args[0]);
			openvr_matrix_from_transform(&matrix, &gmatrix, arvr_api->godot_arvr_get_worldscale());

			if (get_openvr_config_data()->tracking_universe == OpenVRTrackingUniverse::SEATED) {
				origin = vr::TrackingUniverseSeated;
			} else if (get_openvr_config_data()->tracking_universe == OpenVRTrackingUniverse::STANDING) {
				origin = vr::TrackingUniverseStanding;
			} else {
				origin = vr::TrackingUniverseRawAndUncalibrated;
			}

			vr::EVROverlayError vrerr =  vr::VROverlay()->SetOverlayTransformAbsolute(openvr_overlay_data->ovr->overlay, origin, &matrix);

			if (vrerr != vr::VROverlayError_None) {
				printf("Could not track overlay absolute, OpenVR error: %i, %s\n", vrerr, vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr));

				api->godot_variant_new_bool(&ret, false);
				return ret;
			}
        }
    }

	api->godot_variant_new_bool(&ret, true);
	return ret;
}

