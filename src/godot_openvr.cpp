////////////////////////////////////////////////////////////////////////////
// OpenVR GDNative module for Godot
//
// Written by Bastiaan "Mux213" Olij, with loads of help from Thomas "Karroffel" Herzog

// #include <gdnative_api_struct.h>
#include <gdnative_api_struct.gen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openvr.h>

////////////////////////////////////////////////////////////////////////
// gdnative init

#ifdef __cplusplus
extern "C" {
#endif

void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options *p_options);
void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options *p_options);
void GDN_EXPORT godot_nativescript_init(void *p_handle);
void GDN_EXPORT *godot_arvr_constructor(godot_object *p_instance);
void GDN_EXPORT godot_arvr_destructor(void *p_data);
godot_string GDN_EXPORT godot_arvr_get_name(void *p_data);
godot_int GDN_EXPORT godot_arvr_get_capabilities(void *p_data);
godot_bool GDN_EXPORT godot_arvr_get_anchor_detection_is_enabled(void *p_data);
void GDN_EXPORT godot_arvr_set_anchor_detection_is_enabled(void *p_data, bool p_enable);
godot_bool GDN_EXPORT godot_arvr_is_stereo(void *p_data);
godot_bool GDN_EXPORT godot_arvr_is_initialized(void *p_data);
godot_bool GDN_EXPORT godot_arvr_initialize(void *p_data);
void GDN_EXPORT godot_arvr_uninitialize(void *p_data);
godot_vector2 GDN_EXPORT godot_arvr_get_recommended_render_targetsize(void *p_data);
godot_transform GDN_EXPORT godot_arvr_get_transform_for_eye(void *p_data, godot_int p_eye, godot_transform *p_cam_transform);
void GDN_EXPORT godot_arvr_fill_projection_for_eye(void *p_data, godot_real *p_projection, godot_int p_eye, godot_real p_aspect, godot_real p_z_near, godot_real p_z_far);
void GDN_EXPORT godot_arvr_commit_for_eye(void *p_data, godot_int p_eye, godot_rid *p_render_target, godot_rect2 *p_screen_rect);
void GDN_EXPORT godot_arvr_process(void *p_data);

#ifdef __cplusplus
}
#endif

const godot_gdnative_api_struct *api = NULL;

void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options *p_options) {
	api = p_options->api_struct;
}

void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options *p_options) {
	api = NULL;
}

void GDN_EXPORT godot_nativescript_init(void *p_handle) {

}

////////////////////////////////////////////////////////////////////////
// arvr openvr module

typedef struct arvr_data_struct {
	vr::IVRSystem *hmd;
	vr::IVRRenderModels *render_models;
	vr::TrackedDevicePose_t tracked_device_pose[vr::k_unMaxTrackedDeviceCount];
	vr::VRControllerState_t tracked_device_state[vr::k_unMaxTrackedDeviceCount];
	godot_int trackers[vr::k_unMaxTrackedDeviceCount];
	godot_transform hmd_transform;
} arvr_data_struct;

void openvr_attach_device(arvr_data_struct *p_arvr_data, uint32_t p_device_index) {
	// later
};

void openvr_detach_device(arvr_data_struct *p_arvr_data, uint32_t p_device_index) {
	// later
};

void openvr_transform_from_matrix(godot_transform *p_dest, vr::HmdMatrix34_t *p_matrix, godot_real p_world_scale) {
	godot_basis basis;
	godot_vector3 origin;
	float *basis_ptr = (float *) &basis; // Godot can switch between real_t being double or float.. which one is used...

	int k = 0;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			basis_ptr[k++] = p_matrix->m[i][j];
		};
	};

	api->godot_vector3_new(&origin, p_matrix->m[0][3] * p_world_scale, p_matrix->m[1][3] * p_world_scale, p_matrix->m[2][3] * p_world_scale);
	api->godot_transform_new(p_dest, &basis, &origin);
}

void GDN_EXPORT *godot_arvr_constructor(godot_object *p_instance) {
	godot_string ret;

	arvr_data_struct *arvr_data = (arvr_data_struct *)api->godot_alloc(sizeof(arvr_data_struct));
	arvr_data->hmd = NULL;
	arvr_data->render_models = NULL;
	api->godot_transform_new_identity(&arvr_data->hmd_transform);

	return arvr_data;
}

void GDN_EXPORT godot_arvr_destructor(void *p_data) {
	if (p_data != NULL) {
		arvr_data_struct *arvr_data = (arvr_data_struct *)p_data;
		if (arvr_data->hmd != NULL) {
			// this should have already been called... But just in case...
			godot_arvr_uninitialize(p_data);
		}

		api->godot_free(p_data);
	}
}

godot_string GDN_EXPORT godot_arvr_get_name(void *p_data) {
	godot_string ret;

	char name[] = "OpenVR";
	api->godot_string_new_data(&ret, name, strlen(name));

	return ret;
}

godot_int GDN_EXPORT godot_arvr_get_capabilities(void *p_data) {
	godot_int ret;

	ret = 2 + 8; // 2 = ARVR_STEREO, 8 = ARVR_EXTERNAL

	return ret;
};

godot_bool GDN_EXPORT godot_arvr_get_anchor_detection_is_enabled(void *p_data) {
	godot_bool ret;

	ret = false; // does not apply here

	return ret;
};

void GDN_EXPORT godot_arvr_set_anchor_detection_is_enabled(void *p_data, bool p_enable) {
	// we ignore this, not supported in this interface!
};

godot_bool GDN_EXPORT godot_arvr_is_stereo(void *p_data) {
	godot_bool ret;

	ret = true;

	return ret;
};

godot_bool GDN_EXPORT godot_arvr_is_initialized(void *p_data) {
	godot_bool ret;
	arvr_data_struct *arvr_data = (arvr_data_struct *)p_data;

	ret = arvr_data == NULL ? false : arvr_data->hmd != NULL;

	return ret;
};
 
godot_bool GDN_EXPORT godot_arvr_initialize(void *p_data) {
	godot_bool ret;
	arvr_data_struct * arvr_data = (arvr_data_struct *) p_data;

	if (arvr_data->hmd == NULL) {
		// initialise this interface, so initialize any 3rd party libraries, open up HMD window if required, etc.

		bool success = true;
		vr::EVRInitError error = vr::VRInitError_None;

		// reset some stuff
		for (uint32_t i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
			arvr_data->trackers[i] = 0;
		};

		if (!vr::VR_IsRuntimeInstalled()) {
			printf("SteamVR has not been installed.\n");
			success = false;
		};

		if (success) {
			// Loading the SteamVR Runtime
			arvr_data->hmd = vr::VR_Init(&error, vr::VRApplication_Scene);

			if (error != vr::VRInitError_None) {
				success = false;
				printf("Unable to init VR runtime: %s\n", vr::VR_GetVRInitErrorAsEnglishDescription(error));
			} else {
				printf("Main OpenVR interface has been initialized\n");
			};
		};

		if (success) {
			// render models give us access to mesh representations of the various controllers
			arvr_data->render_models = (vr::IVRRenderModels *)vr::VR_GetGenericInterface(vr::IVRRenderModels_Version, &error);
			if (!arvr_data->render_models) {
				success = false;

				printf("Unable to get render model interface: %s\n",vr::VR_GetVRInitErrorAsEnglishDescription(error));
			} else {
				printf("Main render models interface has been initialized.\n");
			};
		};

		if (!vr::VRCompositor()) {
			success = false;

			printf("Compositor initialization failed. See log file for details.\n");
		};

		if (success) {
			// find any already attached devices
			for (uint32_t i = vr::k_unTrackedDeviceIndex_Hmd; i < vr::k_unMaxTrackedDeviceCount; i++) {
				if (arvr_data->hmd->IsTrackedDeviceConnected(i)) {
					openvr_attach_device(arvr_data, i);
				};
			};
		};

		if (!success) {
			godot_arvr_uninitialize(p_data);
		};

		// note, this will be made the primary interface by ARVRInterfaceGDNative
	};

	// and return our result
	ret = arvr_data->hmd != NULL;
	return ret;
};

void GDN_EXPORT godot_arvr_uninitialize(void *p_data) {
	arvr_data_struct * arvr_data = (arvr_data_struct *) p_data;

	if (arvr_data->hmd != NULL) {
		// note, this will already be removed as the primary interface by ARVRInterfaceGDNative

		// detach all our divices
		for (uint32_t i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
			openvr_detach_device(arvr_data, i);
		};

		vr::VR_Shutdown();
		arvr_data->hmd = NULL;
		arvr_data->render_models = NULL;
	};
};

godot_vector2 GDN_EXPORT godot_arvr_get_recommended_render_targetsize(void *p_data) {
	arvr_data_struct * arvr_data = (arvr_data_struct *) p_data;
	godot_vector2 size;

	if (arvr_data->hmd != NULL) {
		uint32_t width, height;

		arvr_data->hmd->GetRecommendedRenderTargetSize(&width, &height);

		api->godot_vector2_new(&size, width, height);
	} else {
		api->godot_vector2_new(&size, 500, 500);
	};

	return size;
};

godot_transform GDN_EXPORT godot_arvr_get_transform_for_eye(void *p_data, godot_int p_eye, godot_transform *p_cam_transform) {
	arvr_data_struct * arvr_data = (arvr_data_struct *) p_data;
	godot_transform transform_for_eye;
	godot_transform reference_frame = api->godot_arvr_get_reference_frame();
	godot_transform ret;
	godot_vector3 offset;
	godot_real world_scale = api->godot_arvr_get_worldscale();

	if (p_eye == 0) {
		// we want a monoscopic transform.. shouldn't really apply here
		api->godot_transform_new_identity(&transform_for_eye);
	} else if (arvr_data->hmd != NULL) {
		vr::HmdMatrix34_t matrix = arvr_data->hmd->GetEyeToHeadTransform(p_eye == 1 ? vr::Eye_Left : vr::Eye_Right);

		openvr_transform_from_matrix(&transform_for_eye, &matrix, world_scale);
/*
		transform_for_eye.basis.set(
				matrix.m[0][0], matrix.m[0][1], matrix.m[0][2],
				matrix.m[1][0], matrix.m[1][1], matrix.m[1][2],
				matrix.m[2][0], matrix.m[2][1], matrix.m[2][2]);

		transform_for_eye.origin.x = matrix.m[0][3] * world_scale;
		transform_for_eye.origin.y = matrix.m[1][3] * world_scale;
		transform_for_eye.origin.z = matrix.m[2][3] * world_scale;
*/
	} else {
		// really not needed, just being paranoid..
		godot_vector3 offset;
		api->godot_transform_new_identity(&transform_for_eye);
		if (p_eye == 1) {
			api->godot_vector3_new(&offset, -0.035 * world_scale, 0.0, 0.0);
		} else {
			api->godot_vector3_new(&offset, 0.035 * world_scale, 0.0, 0.0);
		};
		api->godot_transform_translated(&transform_for_eye, &offset);
	};

	// Now construct our full transform, the order may be in reverse, have to test :)
	ret = *p_cam_transform;
	ret = api->godot_transform_operator_multiply(&ret, &reference_frame);
	ret = api->godot_transform_operator_multiply(&ret, &arvr_data->hmd_transform);
	ret = api->godot_transform_operator_multiply(&ret, &transform_for_eye);

	return ret;
};

void GDN_EXPORT godot_arvr_fill_projection_for_eye(void *p_data, godot_real *p_projection, godot_int p_eye, godot_real p_aspect, godot_real p_z_near, godot_real p_z_far) {
	arvr_data_struct * arvr_data = (arvr_data_struct *) p_data;

	if (arvr_data->hmd != NULL) {
		vr::HmdMatrix44_t matrix = arvr_data->hmd->GetProjectionMatrix(p_eye == 1 ? vr::Eye_Left : vr::Eye_Right, p_z_near, p_z_far);

		int k = 0;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				p_projection[k++] = matrix.m[j][i];
//				camera.matrix[i][j] = matrix.m[j][i];
			};
		};
	} else {
		// uhm, should do something here really..
	};
};

void GDN_EXPORT godot_arvr_commit_for_eye(void *p_data, godot_int p_eye, godot_rid *p_render_target, godot_rect2 *p_screen_rect) {
	arvr_data_struct * arvr_data = (arvr_data_struct *) p_data;

	// This function is responsible for outputting the final render buffer for each eye. 
	// p_screen_rect will only have a value when we're outputting to the main viewport.

	// For an interface that must output to the main viewport (such as with mobile VR) we should give an error when p_screen_rect is not set
	// For an interface that outputs to an external device we should render a copy of one of the eyes to the main viewport if p_screen_rect is set, and only output to the external device if not.

	if (p_eye == 1 && !api->godot_rect2_has_no_area(p_screen_rect)) {
		// blit as mono
		api->godot_arvr_blit(0, p_render_target, p_screen_rect);
	};

	if (arvr_data->hmd != NULL) {
		vr::VRTextureBounds_t bounds;
		bounds.uMin = 0.0;
		bounds.uMax = 1.0;
		bounds.vMin = 0.0;
		bounds.vMax = 1.0;

		uint32_t texid = api->godot_arvr_get_texid(p_render_target);

		vr::Texture_t eyeTexture = { (void *)(uintptr_t)texid, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
		vr::EVRCompositorError vrerr = vr::VRCompositor()->Submit(p_eye == 1 ? vr::Eye_Left : vr::Eye_Right, &eyeTexture, &bounds);
		if (vrerr != vr::VRCompositorError_None) {
			printf("OpenVR reports: %i\n", vrerr);
		}
	};
};

void GDN_EXPORT godot_arvr_process(void *p_data) {
	arvr_data_struct * arvr_data = (arvr_data_struct *) p_data;

	// this method gets called before every frame is rendered, here is where you should update tracking data, update controllers, etc.
	if (arvr_data->hmd != NULL) {
		// Process SteamVR events
		vr::VREvent_t event;
		while (arvr_data->hmd->PollNextEvent(&event, sizeof(event))) {
			switch (event.eventType) {
				case vr::VREvent_TrackedDeviceActivated: {
					openvr_attach_device(arvr_data, event.trackedDeviceIndex);
				}; break;
				case vr::VREvent_TrackedDeviceDeactivated: {
					openvr_detach_device(arvr_data, event.trackedDeviceIndex);
				}; break;
				default: {
					// ignored for now...
				}; break;
			};
		};

		///@TODO we should time how long it takes between calling WaitGetPoses and committing the output to the HMD and using that as the 4th parameter...

		// update our poses structure, this tracks our controllers
		vr::VRCompositor()->WaitGetPoses(arvr_data->tracked_device_pose, vr::k_unMaxTrackedDeviceCount, NULL, 0);

		// we scale all our positions by our world scale
		godot_real world_scale = api->godot_arvr_get_worldscale();

		// update trackers and joysticks
		for (uint32_t i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
			// update tracker
			if (arvr_data->tracked_device_pose[i].bPoseIsValid) {
				// bit wasteful copying it but I don't want to type so much!
				vr::HmdMatrix34_t matPose = arvr_data->tracked_device_pose[i].mDeviceToAbsoluteTracking;

/*
				Basis orientation;
				orientation.set(
						matPose.m[0][0], matPose.m[0][1], matPose.m[0][2],
						matPose.m[1][0], matPose.m[1][1], matPose.m[1][2],
						matPose.m[2][0], matPose.m[2][1], matPose.m[2][2]);

				Vector3 position;
				position.x = matPose.m[0][3];
				position.y = matPose.m[1][3];
				position.z = matPose.m[2][3];
*/

				if (i == 0) {
					// store our HMD transform
					openvr_transform_from_matrix(&arvr_data->hmd_transform, &matPose, world_scale);
				} else if (arvr_data->trackers[i] != NULL) {
					godot_transform transform;
					openvr_transform_from_matrix(&transform, &matPose, 1.0);
//					api->godot_arvr_set_tracker_transform(trackers[i], &transform);
//					trackers[i]->set_orientation(orientation);
//					trackers[i]->set_rw_position(position);

					// update our button state structure
					vr::VRControllerState_t new_state;
					arvr_data->hmd->GetControllerState(i, &new_state, sizeof(vr::VRControllerState_t));
					if (arvr_data->tracked_device_state[i].unPacketNum != new_state.unPacketNum) {
						// we currently have 8 defined buttons on VIVE controllers.
/*
						for (int button = 0; button < 8; button++) {
							api->godot_arvr_set_controller_button(trackers[i], button, new_state.ulButtonPressed & vr::ButtonMaskFromId((vr::EVRButtonId)button));
//							input->joy_button(joyid, button, new_state.ulButtonPressed & vr::ButtonMaskFromId((vr::EVRButtonId)button));
						};

						// support 3 axis for now, this may need to be enhanced
						InputDefault::JoyAxis jx;
						jx.min = -1;
						jx.value = new_state.rAxis[vr::k_EButton_SteamVR_Touchpad].x;
//						input->joy_axis(joyid, JOY_AXIS_0, jx);
						api->godot_arvr_set_controller_axis(trackers[i], JOY_AXIS_0, jx);

						jx.value = new_state.rAxis[vr::k_EButton_SteamVR_Touchpad].y;
//						input->joy_axis(joyid, JOY_AXIS_1, jx);
						api->godot_arvr_set_controller_axis(trackers[i], JOY_AXIS_1, jx);

						jx.min = 0;
						jx.value = new_state.rAxis[vr::k_EButton_SteamVR_Touchpad].x;
//						input->joy_axis(joyid, JOY_AXIS_2, jx);
						api->godot_arvr_set_controller_axis(trackers[i], JOY_AXIS_2, jx);
*/
						arvr_data->tracked_device_state[i] = new_state;
					};
				};
			};
		};
	};


};
