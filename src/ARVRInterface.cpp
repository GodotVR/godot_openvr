////////////////////////////////////////////////////////////////////////////////////////////////
// Our main ARVRInterface code for our OpenVR GDNative module

// Note, even though this is pure C code, we're using the C++ compiler as
// Microsoft never updated their C compiler to understand more modern dialects
// and openvr uses pesky things such as namespaces

#include "ARVRInterface.h"
#include "OS.hpp"
#include "VisualServer.hpp"

void godot_attach_device(arvr_data_struct *p_arvr_data, uint32_t p_device_index) {
	if (p_device_index == vr::k_unTrackedDeviceIndex_Hmd) {
		// we no longer track our HMD, this is all handled in ARVROrigin :)
	} else if (p_device_index == vr::k_unTrackedDeviceIndexInvalid) {
		// really?!
	} else if (p_arvr_data->trackers[p_device_index] == 0) {
		char device_name[256];
		strcpy(device_name, p_arvr_data->ovr->get_device_name(p_device_index, 255));

		vr::TrackedDeviceClass device_class = p_arvr_data->ovr->get_tracked_device_class(p_device_index);
		if (device_class == vr::TrackedDeviceClass_TrackingReference) {
			// ignore base stations and cameras for now
			printf("Found base station %i (%s)\n", p_device_index, device_name);
		} else if (device_class == vr::TrackedDeviceClass_HMD) {
			// ignore any HMD
			printf("Found HMD %i (%s)\n", p_device_index, device_name);
		} else {
			godot_int hand = 0;

			if (device_class == vr::TrackedDeviceClass_Controller) {
				printf("Found controller %i (%s)\n", p_device_index, device_name);

				// If this is a controller than get our controller role
				int32_t controllerRole = p_arvr_data->ovr->get_controller_role(p_device_index);
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
			} else {
				printf("Found tracker %i (%s)\n", p_device_index, device_name);
			}

			sprintf(&device_name[strlen(device_name)], "_%i", p_device_index);
			p_arvr_data->trackers[p_device_index] = godot::arvr_api->godot_arvr_add_controller(device_name, hand, true, true);

			// remember our primary left and right hand devices
			if ((hand == 1) && (p_arvr_data->left_hand_device == vr::k_unTrackedDeviceIndexInvalid)) {
				p_arvr_data->left_hand_device = p_device_index;
			} else if ((hand == 2) && (p_arvr_data->right_hand_device == vr::k_unTrackedDeviceIndexInvalid)) {
				p_arvr_data->right_hand_device = p_device_index;
			}
		}
	}
}

void godot_detach_device(arvr_data_struct *p_arvr_data, uint32_t p_device_index) {
	if (p_device_index == vr::k_unTrackedDeviceIndexInvalid) {
		// really?!
	} else if (p_arvr_data->trackers[p_device_index] != 0) {
		godot::arvr_api->godot_arvr_remove_controller(p_arvr_data->trackers[p_device_index]);
		p_arvr_data->trackers[p_device_index] = 0;

		// unset left/right hand devices
		if (p_arvr_data->left_hand_device == p_device_index) {
			p_arvr_data->left_hand_device = vr::k_unTrackedDeviceIndexInvalid;
		} else if (p_arvr_data->right_hand_device == p_device_index) {
			p_arvr_data->right_hand_device = vr::k_unTrackedDeviceIndexInvalid;
		}
	}
}

godot_string godot_arvr_get_name(const void *p_data) {
	godot_string ret;

	char name[] = "OpenVR";
	godot::api->godot_string_new(&ret);
	godot::api->godot_string_parse_utf8(&ret, name);

	return ret;
}

godot_int godot_arvr_get_capabilities(const void *p_data) {
	godot_int ret;

	ret = 2 + 8; // 2 = ARVR_STEREO, 8 = ARVR_EXTERNAL

	return ret;
}

godot_bool godot_arvr_get_anchor_detection_is_enabled(const void *p_data) {
	godot_bool ret;

	ret = false; // does not apply here

	return ret;
}

void godot_arvr_set_anchor_detection_is_enabled(void *p_data, bool p_enable) {
	// we ignore this, not supported in this interface!
}

godot_bool godot_arvr_is_stereo(const void *p_data) {
	godot_bool ret;

	ret = true;

	return ret;
}

godot_bool godot_arvr_is_initialized(const void *p_data) {
	arvr_data_struct *arvr_data = (arvr_data_struct *)p_data;

	if (arvr_data == NULL) {
		return false;
	}

	if (arvr_data->ovr == NULL) {
		return false;
	}

	return arvr_data->ovr->is_initialised();
}

godot_bool godot_arvr_initialize(void *p_data) {
	arvr_data_struct *arvr_data = (arvr_data_struct *)p_data;

	// this should be static once Godot runs but obtain whether we're running GLES2, GLES3 or Vulkan
	arvr_data->video_driver = godot::OS::get_singleton()->get_current_video_driver();

	if (arvr_data->ovr->initialise()) {
		// reset some stuff
		for (uint32_t i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
			arvr_data->trackers[i] = 0;
			arvr_data->last_rumble_update[i] = 0;
		}

		// init our controller tracking variables
		arvr_data->device_hands_are_available = false;
		arvr_data->left_hand_device = vr::k_unTrackedDeviceIndexInvalid;
		arvr_data->right_hand_device = vr::k_unTrackedDeviceIndexInvalid;

		// find any already attached devices
		for (uint32_t i = vr::k_unTrackedDeviceIndex_Hmd; i < vr::k_unMaxTrackedDeviceCount; i++) {
			if (arvr_data->ovr->is_tracked_device_connected(i)) {
				godot_attach_device(arvr_data, i);
			}
		}

		// go and get our recommended target size
		arvr_data->ovr->get_recommended_rendertarget_size(&arvr_data->width, &arvr_data->height);

		// note, this will be made the primary interface by ARVRInterfaceGDNative
	}

	// and return our result
	return arvr_data->ovr->is_initialised();
}

void godot_arvr_uninitialize(void *p_data) {
	arvr_data_struct *arvr_data = (arvr_data_struct *)p_data;

	// note, this will already be removed as the primary interface by
	// ARVRInterfaceGDNative

	// detach all our divices
	for (uint32_t i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
		godot_detach_device(arvr_data, i);
	}
}

godot_vector2 godot_arvr_get_render_targetsize(const void *p_data) {
	arvr_data_struct *arvr_data = (arvr_data_struct *)p_data;
	godot_vector2 size;

	if (arvr_data->ovr->is_initialised()) {
		// TODO: we should periodically check if the recommended size has changed (the user can adjust this) and if so update our width/height
		// and reset our render texture (RID)

		godot::api->godot_vector2_new(&size, arvr_data->width, arvr_data->height);
	} else {
		godot::api->godot_vector2_new(&size, 500, 500);
	}

	return size;
}

godot_transform godot_arvr_get_transform_for_eye(void *p_data, godot_int p_eye, godot_transform *p_cam_transform) {
	arvr_data_struct *arvr_data = (arvr_data_struct *)p_data;

	// todo - rewrite this to use Transform object

	godot_transform transform_for_eye;
	godot_transform reference_frame = godot::arvr_api->godot_arvr_get_reference_frame();
	godot_transform ret;
	godot_real world_scale = godot::arvr_api->godot_arvr_get_worldscale();

	if (p_eye == 0) {
		// we want a monoscopic transform.. shouldn't really apply here
		godot::api->godot_transform_new_identity(&transform_for_eye);
	} else if (arvr_data->ovr != NULL) {
		arvr_data->ovr->get_eye_to_head_transform(&transform_for_eye, p_eye, world_scale);
	} else {
		// really not needed, just being paranoid..
		godot_vector3 offset;
		godot::api->godot_transform_new_identity(&transform_for_eye);
		if (p_eye == 1) {
			godot::api->godot_vector3_new(&offset, -0.035 * world_scale, 0.0, 0.0);
		} else {
			godot::api->godot_vector3_new(&offset, 0.035 * world_scale, 0.0, 0.0);
		};
		godot::api->godot_transform_translated(&transform_for_eye, &offset);
	};

	// Now construct our full transform, the order may be in reverse, have to test
	// :)
	ret = *p_cam_transform;
	ret = godot::api->godot_transform_operator_multiply(&ret, &reference_frame);
	ret = godot::api->godot_transform_operator_multiply(&ret, &arvr_data->hmd_transform);
	ret = godot::api->godot_transform_operator_multiply(&ret, &transform_for_eye);

	return ret;
}

void godot_arvr_fill_projection_for_eye(void *p_data, godot_real *p_projection, godot_int p_eye, godot_real p_aspect, godot_real p_z_near, godot_real p_z_far) {
	arvr_data_struct *arvr_data = (arvr_data_struct *)p_data;

	if (arvr_data->ovr->is_initialised()) {
		vr::HmdMatrix44_t matrix = arvr_data->ovr->hmd->GetProjectionMatrix(
				p_eye == 1 ? vr::Eye_Left : vr::Eye_Right, p_z_near, p_z_far);

		int k = 0;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				p_projection[k++] = matrix.m[j][i];
			}
		}
	} else {
		// uhm, should do something here really..
	}
}

void godot_arvr_commit_for_eye(void *p_data, godot_int p_eye, godot_rid *p_render_target, godot_rect2 *p_screen_rect) {
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

	godot::Rect2 screen_rect = *(godot::Rect2 *)p_screen_rect;

	if (p_eye == 1 && !screen_rect.has_no_area()) {
		// blit as mono, attempt to keep our aspect ratio and center our render buffer
		godot_vector2 rs = godot_arvr_get_render_targetsize(p_data);
		godot::Vector2 *render_size = (godot::Vector2 *)&rs;

		float new_height = screen_rect.size.x * (render_size->y / render_size->x);
		if (new_height > screen_rect.size.y) {
			screen_rect.position.y = (0.5 * screen_rect.size.y) - (0.5 * new_height);
			screen_rect.size.y = new_height;
		} else {
			float new_width = screen_rect.size.y * (render_size->x / render_size->y);

			screen_rect.position.x = (0.5 * screen_rect.size.x) - (0.5 * new_width);
			screen_rect.size.x = new_width;
		}

		// printf("Blit: %0.2f, %0.2f - %0.2f, %0.2f\n",screen_rect.position.x,screen_rect.position.y,screen_rect.size.x,screen_rect.size.y);

		godot::arvr_api->godot_arvr_blit(0, p_render_target, (godot_rect2 *)&screen_rect);
	}

	if (arvr_data->ovr->is_initialised()) {
		vr::VRTextureBounds_t bounds;
		bounds.uMin = 0.0;
		bounds.uMax = 1.0;
		bounds.vMin = 0.0;
		bounds.vMax = 1.0;

		uint32_t texid = godot::arvr_api->godot_arvr_get_texid(p_render_target);

		vr::Texture_t eyeTexture = { (void *)(uintptr_t)texid, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };

		if (arvr_data->ovr->get_application_type() == openvr_data::OpenVRApplicationType::OVERLAY) {
			// Overlay mode
			if (p_eye == 1) {
				vr::EVROverlayError vrerr;

				for (unsigned i = 0; i < arvr_data->ovr->get_overlay_count(); i++) {
					vr::TextureID_t texidov = godot::VisualServer::get_singleton()->texture_get_texid(godot::VisualServer::get_singleton()->viewport_get_texture(arvr_data->ovr->get_overlay(i).viewport_rid));

					if (texid == texidov) {
						vrerr = vr::VROverlay()->SetOverlayTexture(arvr_data->ovr->get_overlay(i).handle, &eyeTexture);

						if (vrerr != vr::VROverlayError_None) {
							printf("OpenVR could not set texture for overlay: %i, %s\n", vrerr, vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr));
						}

						vrerr = vr::VROverlay()->SetOverlayTextureBounds(arvr_data->ovr->get_overlay(i).handle, &bounds);

						if (vrerr != vr::VROverlayError_None) {
							printf("OpenVR could not set textute bounds for overlay: %i, %s\n", vrerr, vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr));
						}
					}
				}
			}
		} else {
			vr::EVRCompositorError vrerr = vr::VRCompositor()->Submit(p_eye == 1 ? vr::Eye_Left : vr::Eye_Right, &eyeTexture, &bounds);
			if (vrerr != vr::VRCompositorError_None) {
				printf("OpenVR reports: %i\n", vrerr);
			}
		}
	}
}

void godot_arvr_process(void *p_data) {
	arvr_data_struct *arvr_data = (arvr_data_struct *)p_data;

	// this method gets called before every frame is rendered, here is where you
	// should update tracking data, update controllers, etc.
	if (arvr_data->ovr->is_initialised()) {
		// we need timing info for one or two things..
		uint64_t msec = godot::OS::get_singleton()->get_ticks_msec();

		// Call process on our ovr system.
		// We should move a lot more in here! For now it just handles our async model loads
		arvr_data->ovr->process();

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
					if (event.trackedDeviceIndex != vr::k_unTrackedDeviceIndexInvalid) {
						int button = int(event.data.controller.button);
						if (button == vr::k_EButton_SteamVR_Touchpad) {
							// If the button being pressed is the Touchpad, reassign it to button 14
							button = 14;
						} else if (button == vr::k_EButton_SteamVR_Trigger) {
							// If the button being pressed is the trigger, reassign it to button 15
							button = 15;
						}
						godot::arvr_api->godot_arvr_set_controller_button(arvr_data->trackers[event.trackedDeviceIndex], button, true);
					}
				}; break;
				case vr::VREvent_ButtonUnpress: {
					if (event.trackedDeviceIndex != vr::k_unTrackedDeviceIndexInvalid) {
						int button = int(event.data.controller.button);
						//Do that again when the button is released
						if (button == vr::k_EButton_SteamVR_Touchpad) {
							button = 14;
						} else if (button == vr::k_EButton_SteamVR_Trigger) {
							button = 15;
						}
						godot::arvr_api->godot_arvr_set_controller_button(arvr_data->trackers[event.trackedDeviceIndex], button, false);
					}
				} break;
				default: {
					// ignored for now...
				}; break;
			}
		}

		///@TODO we should time how long it takes between calling WaitGetPoses and
		/// committing the output to the HMD and using that as the 4th parameter...

		// update our poses structure, this tracks our controllers
		if (arvr_data->ovr->get_application_type() == openvr_data::OpenVRApplicationType::OVERLAY) {
			openvr_data::OpenVRTrackingUniverse tracking_universe = arvr_data->ovr->get_tracking_universe();
			if (tracking_universe == openvr_data::OpenVRTrackingUniverse::SEATED) {
				vr::VRSystem()->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseSeated, 0.0, arvr_data->tracked_device_pose, vr::k_unMaxTrackedDeviceCount);
			} else if (tracking_universe == openvr_data::OpenVRTrackingUniverse::STANDING) {
				vr::VRSystem()->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, 0.0, arvr_data->tracked_device_pose, vr::k_unMaxTrackedDeviceCount);
			} else {
				vr::VRSystem()->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseRawAndUncalibrated, 0.0, arvr_data->tracked_device_pose, vr::k_unMaxTrackedDeviceCount);
			}
		} else {
			vr::VRCompositor()->WaitGetPoses(arvr_data->tracked_device_pose, vr::k_unMaxTrackedDeviceCount, NULL, 0);
		}

		// we scale all our positions by our world scale
		godot_real world_scale = godot::arvr_api->godot_arvr_get_worldscale();

		// update trackers and joysticks
		for (uint32_t i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
			// update tracker
			if (arvr_data->tracked_device_pose[i].bPoseIsValid) {
				// bit wasteful copying it but I don't want to type so much!
				vr::HmdMatrix34_t matPose =
						arvr_data->tracked_device_pose[i].mDeviceToAbsoluteTracking;

				if (i == 0) {
					// store our HMD transform
					arvr_data->ovr->transform_from_matrix(&arvr_data->hmd_transform, &matPose, world_scale);
				} else if (arvr_data->trackers[i] != 0) {
					// update our location and orientation
					godot_transform transform;
					arvr_data->ovr->transform_from_matrix(&transform, &matPose, 1.0);
					godot::arvr_api->godot_arvr_set_controller_transform(arvr_data->trackers[i],
							&transform, true, true);

					// update our button state structure
					vr::VRControllerState_t old_state = arvr_data->tracked_device_state[i];
					vr::VRControllerState_t new_state;
					arvr_data->ovr->hmd->GetControllerState(i, &new_state, sizeof(vr::VRControllerState_t));
					if (arvr_data->tracked_device_state[i].unPacketNum != new_state.unPacketNum) {
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
									godot::arvr_api->godot_arvr_set_controller_axis(
											arvr_data->trackers[i], axis * 2, new_state.rAxis[axis].x,
											true); // had some weirdness with this false, I think it may be -1.0 to 1.0 afterall
								}
							} else {
								// this can be trackpad or joystick. Might need to do more with trackpad..
								if (new_state.rAxis[axis].x != old_state.rAxis[axis].x) {
									godot::arvr_api->godot_arvr_set_controller_axis(
											arvr_data->trackers[i], axis * 2, new_state.rAxis[axis].x,
											true);
								}

								if (new_state.rAxis[axis].y != old_state.rAxis[axis].y) {
									godot::arvr_api->godot_arvr_set_controller_axis(
											arvr_data->trackers[i], (axis * 2) + 1,
											new_state.rAxis[axis].y, true);
								}
							}
						}

						arvr_data->tracked_device_state[i] = new_state;
					}

					// update rumble
					float rumble = godot::arvr_api->godot_arvr_get_controller_rumble(arvr_data->trackers[i]);
					if ((rumble > 0.0) && ((msec - arvr_data->last_rumble_update[i]) > 5)) {
						// We should only call this once ever 5ms...
						arvr_data->ovr->hmd->TriggerHapticPulse(i, 0, (rumble * 5000.0));
						arvr_data->last_rumble_update[i] = msec;
					}
				}
			}
		}
	}
}

void *godot_arvr_constructor(godot_object *p_instance) {
	// note, don't do to much here, not much will have been initialised yet...

	arvr_data_struct *arvr_data = (arvr_data_struct *)godot::api->godot_alloc(sizeof(arvr_data_struct));

	arvr_data->ovr = openvr_data::retain_singleton();
	arvr_data->video_driver = 0;
	godot::api->godot_transform_new_identity(&arvr_data->hmd_transform);

	return arvr_data;
}

void godot_arvr_destructor(void *p_data) {
	if (p_data != NULL) {
		arvr_data_struct *arvr_data = (arvr_data_struct *)p_data;
		if (arvr_data->ovr != NULL) {
			// this should have already been called... But just in case...
			godot_arvr_uninitialize(p_data);

			arvr_data->ovr->release();
			arvr_data->ovr = NULL;
		}

		godot::api->godot_free(p_data);
	}
}

int godot_arvr_get_external_texture_for_eye(void *p_data, int p_eye) {

	return 0;
}

void godot_arvr_notification(void *p_data, int p_what) {
	// nothing to do here for now but we should implement this.
}

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
	godot_arvr_process,
	// only available in Godot 3.2+
	godot_arvr_get_external_texture_for_eye,
	godot_arvr_notification
};
