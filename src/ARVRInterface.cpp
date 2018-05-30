////////////////////////////////////////////////////////////////////////////////////////////////
// Our main ARVRInterface code for our OpenVR GDNative module

// Note, even though this is pure C code, we're using the C++ compiler as
// Microsoft never updated their C compiler to understand more modern dialects
// and openvr uses pesky things such as namespaces

#include "ARVRInterface.h"

typedef struct arvr_data_struct {
	openvr_data_struct *ovr;
	vr::TrackedDevicePose_t tracked_device_pose[vr::k_unMaxTrackedDeviceCount];
	vr::VRControllerState_t tracked_device_state[vr::k_unMaxTrackedDeviceCount];
	godot_int trackers[vr::k_unMaxTrackedDeviceCount];
	uint64_t last_rumble_update[vr::k_unMaxTrackedDeviceCount];
	godot_transform hmd_transform;
	bool device_hands_are_available;
	uint32_t left_hand_device;
	uint32_t right_hand_device;
} arvr_data_struct;

void godot_attach_device(arvr_data_struct *p_arvr_data, uint32_t p_device_index) {
	if (p_device_index == vr::k_unTrackedDeviceIndex_Hmd) {
		// we no longer track our HMD, this is all handled in ARVROrigin :)
	} else if (p_arvr_data->trackers[p_device_index] == 0) {
		char device_name[256];
		strcpy(device_name, openvr_get_device_name(p_arvr_data->ovr, p_device_index, 255));
		printf("Found openvr device %i (%s)\n", p_device_index, device_name);

		if (strstr(device_name, "basestation") != NULL) {
			// ignore base stations for now
		} else if (strstr(device_name, "camera") != NULL) {
			// ignore cameras for now
		} else {
			godot_int hand = 0;
			sprintf(&device_name[strlen(device_name)], "_%i", p_device_index);

			// get our controller role
			vr::ETrackedPropertyError error;
			int32_t controllerRole = p_arvr_data->ovr->hmd->GetInt32TrackedDeviceProperty(p_device_index, vr::Prop_ControllerRoleHint_Int32, &error);
			if (controllerRole == vr::TrackedControllerRole_RightHand) {
				hand = 2;
				p_arvr_data->device_hands_are_available = true;
			} else if (controllerRole == vr::TrackedControllerRole_LeftHand) {
				hand = 1;
				p_arvr_data->device_hands_are_available = true;
			} else if (!p_arvr_data->device_hands_are_available) {
				// this definately needs to improve, if we haven't got hand information, our first controller becomes left and our second becomes right
				if (p_arvr_data->left_hand_device == vr::k_unTrackedDeviceIndexInvalid) {
					hand = 1;
				} else if (p_arvr_data->right_hand_device == vr::k_unTrackedDeviceIndexInvalid) {
					hand = 2;
				}
			}

			p_arvr_data->trackers[p_device_index] = arvr_api->godot_arvr_add_controller(device_name, hand, true, true);

			// remember our primary left and right hand devices
			if ((hand == 1) && (p_arvr_data->left_hand_device == vr::k_unTrackedDeviceIndexInvalid)) {
				p_arvr_data->left_hand_device = p_device_index;
			} else if ((hand == 2) && (p_arvr_data->right_hand_device == vr::k_unTrackedDeviceIndexInvalid)) {
				p_arvr_data->right_hand_device = p_device_index;
			};
		};
	};
};

void godot_detach_device(arvr_data_struct *p_arvr_data,
		uint32_t p_device_index) {
	if (p_arvr_data->trackers[p_device_index] != 0) {
		arvr_api->godot_arvr_remove_controller(p_arvr_data->trackers[p_device_index]);
		p_arvr_data->trackers[p_device_index] = 0;

		// unset left/right hand devices
		if (p_arvr_data->left_hand_device == p_device_index) {
			p_arvr_data->left_hand_device = vr::k_unTrackedDeviceIndexInvalid;
		} else if (p_arvr_data->right_hand_device == p_device_index) {
			p_arvr_data->right_hand_device = vr::k_unTrackedDeviceIndexInvalid;
		}
	};
};

godot_string godot_arvr_get_name(const void *p_data) {
	godot_string ret;

	char name[] = "OpenVR";
	api->godot_string_new(&ret);
	api->godot_string_parse_utf8(&ret, name);

	return ret;
}

godot_int godot_arvr_get_capabilities(const void *p_data) {
	godot_int ret;

	ret = 2 + 8; // 2 = ARVR_STEREO, 8 = ARVR_EXTERNAL

	return ret;
};

godot_bool godot_arvr_get_anchor_detection_is_enabled(const void *p_data) {
	godot_bool ret;

	ret = false; // does not apply here

	return ret;
};

void godot_arvr_set_anchor_detection_is_enabled(void *p_data,
		bool p_enable){
	// we ignore this, not supported in this interface!
};

godot_bool godot_arvr_is_stereo(const void *p_data) {
	godot_bool ret;

	ret = true;

	return ret;
};

godot_bool godot_arvr_is_initialized(const void *p_data) {
	godot_bool ret;
	arvr_data_struct *arvr_data = (arvr_data_struct *)p_data;

	ret = arvr_data == NULL ? false : arvr_data->ovr != NULL;

	return ret;
};

godot_bool godot_arvr_initialize(void *p_data) {
	godot_bool ret;
	arvr_data_struct *arvr_data = (arvr_data_struct *)p_data;

	if (arvr_data->ovr == NULL) {
		// initialise this interface, so initialize any 3rd party libraries, open up
		// HMD window if required, etc.

		arvr_data->ovr = openvr_get_data();
		if (arvr_data->ovr != NULL) {
			// reset some stuff
			for (uint32_t i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
				arvr_data->trackers[i] = 0;
				arvr_data->last_rumble_update[i] = 0;
			};

			// init our controller tracking variables
			arvr_data->device_hands_are_available = false;
			arvr_data->left_hand_device = vr::k_unTrackedDeviceIndexInvalid;
			arvr_data->right_hand_device = vr::k_unTrackedDeviceIndexInvalid;

			// find any already attached devices
			for (uint32_t i = vr::k_unTrackedDeviceIndex_Hmd; i < vr::k_unMaxTrackedDeviceCount; i++) {
				if (arvr_data->ovr->hmd->IsTrackedDeviceConnected(i)) {
					godot_attach_device(arvr_data, i);
				};
			};

			// note, this will be made the primary interface by ARVRInterfaceGDNative
		};
	};

	// and return our result
	ret = arvr_data->ovr != NULL;
	return ret;
};

void godot_arvr_uninitialize(void *p_data) {
	arvr_data_struct *arvr_data = (arvr_data_struct *)p_data;

	if (arvr_data->ovr != NULL) {
		// note, this will already be removed as the primary interface by
		// ARVRInterfaceGDNative

		// detach all our divices
		for (uint32_t i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
			godot_detach_device(arvr_data, i);
		};

		openvr_release_data();
		arvr_data->ovr = NULL;
	};
};

godot_vector2 godot_arvr_get_render_targetsize(const void *p_data) {
	arvr_data_struct *arvr_data = (arvr_data_struct *)p_data;
	godot_vector2 size;

	if (arvr_data->ovr != NULL) {
		uint32_t width, height;

		arvr_data->ovr->hmd->GetRecommendedRenderTargetSize(&width, &height);

		api->godot_vector2_new(&size, width, height);
	} else {
		api->godot_vector2_new(&size, 500, 500);
	};

	return size;
};

godot_transform godot_arvr_get_transform_for_eye(
		void *p_data, godot_int p_eye, godot_transform *p_cam_transform) {
	arvr_data_struct *arvr_data = (arvr_data_struct *)p_data;
	godot_transform transform_for_eye;
	godot_transform reference_frame = arvr_api->godot_arvr_get_reference_frame();
	godot_transform ret;
	godot_vector3 offset;
	godot_real world_scale = arvr_api->godot_arvr_get_worldscale();

	if (p_eye == 0) {
		// we want a monoscopic transform.. shouldn't really apply here
		api->godot_transform_new_identity(&transform_for_eye);
	} else if (arvr_data->ovr != NULL) {
		vr::HmdMatrix34_t matrix = arvr_data->ovr->hmd->GetEyeToHeadTransform(
				p_eye == 1 ? vr::Eye_Left : vr::Eye_Right);

		openvr_transform_from_matrix(&transform_for_eye, &matrix, world_scale);
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

	// Now construct our full transform, the order may be in reverse, have to test
	// :)
	ret = *p_cam_transform;
	ret = api->godot_transform_operator_multiply(&ret, &reference_frame);
	ret = api->godot_transform_operator_multiply(&ret, &arvr_data->hmd_transform);
	ret = api->godot_transform_operator_multiply(&ret, &transform_for_eye);

	return ret;
};

void godot_arvr_fill_projection_for_eye(
		void *p_data, godot_real *p_projection, godot_int p_eye,
		godot_real p_aspect, godot_real p_z_near, godot_real p_z_far) {
	arvr_data_struct *arvr_data = (arvr_data_struct *)p_data;

	if (arvr_data->ovr != NULL) {
		vr::HmdMatrix44_t matrix = arvr_data->ovr->hmd->GetProjectionMatrix(
				p_eye == 1 ? vr::Eye_Left : vr::Eye_Right, p_z_near, p_z_far);

		int k = 0;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				p_projection[k++] = matrix.m[j][i];
			};
		};
	} else {
		// uhm, should do something here really..
	};
};

void godot_arvr_commit_for_eye(void *p_data, godot_int p_eye,
		godot_rid *p_render_target,
		godot_rect2 *p_screen_rect) {
	arvr_data_struct *arvr_data = (arvr_data_struct *)p_data;

	// This function is responsible for outputting the final render buffer for
	// each eye.
	// p_screen_rect will only have a value when we're outputting to the main
	// viewport.

	// For an interface that must output to the main viewport (such as with mobile
	// VR) we should give an error when p_screen_rect is not set
	// For an interface that outputs to an external device we should render a copy
	// of one of the eyes to the main viewport if p_screen_rect is set, and only
	// output to the external device if not.

	godot_rect2 screen_rect = *p_screen_rect;

	if (p_eye == 1 && !api->godot_rect2_has_no_area(&screen_rect)) {
		// blit as mono, attempt to keep our aspect ratio and center our render buffer
		godot_vector2 render_size = godot_arvr_get_render_targetsize(p_data);

		float new_height = screen_rect.size.x * (render_size.y / render_size.x);
		if (new_height > screen_rect.size.y) {
			screen_rect.position.y = (0.5 * screen_rect.size.y) - (0.5 * new_height);
			screen_rect.size.y = new_height;
		} else {
			float new_width = screen_rect.size.y * (render_size.x / render_size.y);

			screen_rect.position.x = (0.5 * screen_rect.size.x) - (0.5 * new_width);
			screen_rect.size.x = new_width;
		};

		// printf("Blit: %0.2f, %0.2f - %0.2f, %0.2f\n",screen_rect.position.x,screen_rect.position.y,screen_rect.size.x,screen_rect.size.y);

		arvr_api->godot_arvr_blit(0, p_render_target, &screen_rect);
	};

	if (arvr_data->ovr != NULL) {
		vr::VRTextureBounds_t bounds;
		bounds.uMin = 0.0;
		bounds.uMax = 1.0;
		bounds.vMin = 0.0;
		bounds.vMax = 1.0;

		uint32_t texid = arvr_api->godot_arvr_get_texid(p_render_target);

		vr::Texture_t eyeTexture = { (void *)(uintptr_t)texid,
			vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
		vr::EVRCompositorError vrerr = vr::VRCompositor()->Submit(
				p_eye == 1 ? vr::Eye_Left : vr::Eye_Right, &eyeTexture, &bounds);
		if (vrerr != vr::VRCompositorError_None) {
			printf("OpenVR reports: %i\n", vrerr);
		}
	};
};

void godot_arvr_process(void *p_data) {
	arvr_data_struct *arvr_data = (arvr_data_struct *)p_data;

	// this method gets called before every frame is rendered, here is where you
	// should update tracking data, update controllers, etc.
	if (arvr_data->ovr != NULL) {
		// we need timing info for one or two things..
		uint64_t msec = OS_get_ticks_msec();

		// Process SteamVR events
		vr::VREvent_t event;
		while (arvr_data->ovr->hmd->PollNextEvent(&event, sizeof(event))) {
			switch (event.eventType) {
				case vr::VREvent_TrackedDeviceActivated: {
					godot_attach_device(arvr_data, event.trackedDeviceIndex);
				}; break;
				case vr::VREvent_TrackedDeviceDeactivated: {
					godot_detach_device(arvr_data, event.trackedDeviceIndex);
				}; break;
				// Get buttons from ButtonPress and ButtonUnpress events
				case vr::VREvent_ButtonPress: {
					int button = int(event.data.controller.button);
					if (button == vr::k_EButton_SteamVR_Touchpad) {
						// If the button being pressed is the Touchpad, reassign it to button 14
						button = 14;
					} else if (button == vr::k_EButton_SteamVR_Trigger ) {
						// If the button being pressed is the trigger, reassign it to button 15
						button = 15;
					}
					arvr_api->godot_arvr_set_controller_button(arvr_data->trackers[event.trackedDeviceIndex], button, true);
				}; break;
				case vr::VREvent_ButtonUnpress: {
					int button = int(event.data.controller.button);
					//Do that again when the button is released
					if (button == vr::k_EButton_SteamVR_Touchpad) {
						button = 14;
					} else if (button == vr::k_EButton_SteamVR_Trigger ) {
						button = 15;
					}
					arvr_api->godot_arvr_set_controller_button(arvr_data->trackers[event.trackedDeviceIndex], button, false);
				}break;
				default: {
					// ignored for now...
				}; break;
			};
		};

		///@TODO we should time how long it takes between calling WaitGetPoses and
		/// committing the output to the HMD and using that as the 4th parameter...

		// update our poses structure, this tracks our controllers
		vr::VRCompositor()->WaitGetPoses(arvr_data->tracked_device_pose,
				vr::k_unMaxTrackedDeviceCount, NULL, 0);

		// we scale all our positions by our world scale
		godot_real world_scale = arvr_api->godot_arvr_get_worldscale();

		// update trackers and joysticks
		for (uint32_t i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
			// update tracker
			if (arvr_data->tracked_device_pose[i].bPoseIsValid) {
				// bit wasteful copying it but I don't want to type so much!
				vr::HmdMatrix34_t matPose =
						arvr_data->tracked_device_pose[i].mDeviceToAbsoluteTracking;

				if (i == 0) {
					// store our HMD transform
					openvr_transform_from_matrix(&arvr_data->hmd_transform, &matPose,
							world_scale);
				} else if (arvr_data->trackers[i] != 0) {
					// update our location and orientation
					godot_transform transform;
					openvr_transform_from_matrix(&transform, &matPose, 1.0);
					arvr_api->godot_arvr_set_controller_transform(arvr_data->trackers[i],
							&transform, true, true);

					// update our button state structure
					vr::VRControllerState_t old_state =
							arvr_data->tracked_device_state[i];
					vr::VRControllerState_t new_state;
					arvr_data->ovr->hmd->GetControllerState(i, &new_state,
							sizeof(vr::VRControllerState_t));
					if (arvr_data->tracked_device_state[i].unPacketNum !=
							new_state.unPacketNum) {
						// OpenVR supports 5 axis with x/y, for triggers only x is set
						for (int axis = 0; axis < 5; axis++) {
							vr::EVRControllerAxisType axis_type =
									(vr::EVRControllerAxisType)
											arvr_data->ovr->hmd->GetInt32TrackedDeviceProperty(
													i, (vr::ETrackedDeviceProperty)(
															   vr::Prop_Axis0Type_Int32 + axis));
							if (axis_type == vr::k_eControllerAxis_None) {
								// not applicable for this controller, ignore
							} else if (axis_type == vr::k_eControllerAxis_Trigger) {
								// only x which ranges between 0. and 1.0
								if (new_state.rAxis[axis].x != old_state.rAxis[axis].x) {
									arvr_api->godot_arvr_set_controller_axis(
											arvr_data->trackers[i], axis * 2, new_state.rAxis[axis].x,
											true); // had some weirdness with this false, I think it may be -1.0 to 1.0 afterall
								};
							} else {
								// this can be trackpad or joystick. Might need to do more with trackpad..
								if (new_state.rAxis[axis].x != old_state.rAxis[axis].x) {
									arvr_api->godot_arvr_set_controller_axis(
											arvr_data->trackers[i], axis * 2, new_state.rAxis[axis].x,
											true);
								};

								if (new_state.rAxis[axis].y != old_state.rAxis[axis].y) {
									arvr_api->godot_arvr_set_controller_axis(
											arvr_data->trackers[i], (axis * 2) + 1,
											new_state.rAxis[axis].y, true);
								};
							};
						};

						arvr_data->tracked_device_state[i] = new_state;
					};

					// update rumble
					float rumble = arvr_api->godot_arvr_get_controller_rumble(arvr_data->trackers[i]);
					if ((rumble > 0.0) && ((msec - arvr_data->last_rumble_update[i]) > 5)) {
						// We should only call this once ever 5ms...
						arvr_data->ovr->hmd->TriggerHapticPulse(i, 0, (rumble * 5000.0));
						arvr_data->last_rumble_update[i] = msec;
					};
				};
			};
		};
	};
};

void *godot_arvr_constructor(godot_object *p_instance) {
	godot_string ret;

	arvr_data_struct *arvr_data = (arvr_data_struct *)api->godot_alloc(sizeof(arvr_data_struct));
	arvr_data->ovr = NULL;
	api->godot_transform_new_identity(&arvr_data->hmd_transform);

	return arvr_data;
};

void godot_arvr_destructor(void *p_data) {
	if (p_data != NULL) {
		arvr_data_struct *arvr_data = (arvr_data_struct *)p_data;
		if (arvr_data->ovr != NULL) {
			// this should have already been called... But just in case...
			godot_arvr_uninitialize(p_data);
		}

		api->godot_free(p_data);
	};
};

const godot_arvr_interface_gdnative interface_struct = {
	GODOTVR_API_MAJOR, GODOTVR_API_MINOR,
	godot_arvr_constructor,
	godot_arvr_destructor,
	godot_arvr_get_name,
	godot_arvr_get_capabilities,
	godot_arvr_get_anchor_detection_is_enabled,
	godot_arvr_set_anchor_detection_is_enabled,
	godot_arvr_is_stereo,
	godot_arvr_is_initialized,
	godot_arvr_initialize,
	godot_arvr_uninitialize,
	godot_arvr_get_render_targetsize,
	godot_arvr_get_transform_for_eye,
	godot_arvr_fill_projection_for_eye,
	godot_arvr_commit_for_eye,
	godot_arvr_process
};
