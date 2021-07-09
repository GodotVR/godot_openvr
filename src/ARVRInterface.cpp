////////////////////////////////////////////////////////////////////////////////////////////////
// Our main ARVRInterface code for our OpenVR GDNative module

// Note, even though this is pure C code, we're using the C++ compiler as
// Microsoft never updated their C compiler to understand more modern dialects
// and openvr uses pesky things such as namespaces

#include "ARVRInterface.h"
#include "VisualServer.hpp"

////////////////////////////////////////////////////////////////
// Returns the name of this interface
godot_string godot_arvr_get_name(const void *p_data) {
	godot_string ret;

	char name[] = "OpenVR";
	godot::api->godot_string_new(&ret);
	godot::api->godot_string_parse_utf8(&ret, name);

	return ret;
}

////////////////////////////////////////////////////////////////
// Returns capabilities for this interface
godot_int godot_arvr_get_capabilities(const void *p_data) {
	godot_int ret;

	ret = 2 + 8; // 2 = ARVR_STEREO, 8 = ARVR_EXTERNAL

	return ret;
}

////////////////////////////////////////////////////////////////
// This is an AR feature not used here
godot_bool godot_arvr_get_anchor_detection_is_enabled(const void *p_data) {
	godot_bool ret;

	ret = false; // does not apply here

	return ret;
}

////////////////////////////////////////////////////////////////
// This is an AR feature not used here
void godot_arvr_set_anchor_detection_is_enabled(void *p_data, bool p_enable) {
	// we ignore this, not supported in this interface!
}

////////////////////////////////////////////////////////////////
// Informs Godot stereoscopic rendering is required
godot_bool godot_arvr_is_stereo(const void *p_data) {
	godot_bool ret;

	ret = true;

	return ret;
}

////////////////////////////////////////////////////////////////
// Returns whether our interface was successfully initialised
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

////////////////////////////////////////////////////////////////
// Initialise our interface, sets up OpenVR and starts sending
// output to our HMD
// Note that you should do any configuration using OpenVRConfig
// before initializing the interface.
godot_bool godot_arvr_initialize(void *p_data) {
	arvr_data_struct *arvr_data = (arvr_data_struct *)p_data;

	// this should be static once Godot runs but obtain whether we're running GLES2, GLES3 or Vulkan
	arvr_data->video_driver = godot::OS::get_singleton()->get_current_video_driver();

	if (arvr_data->ovr->initialise()) {
		// go and get our recommended target size
		arvr_data->ovr->get_recommended_rendertarget_size(&arvr_data->width, &arvr_data->height);

		// note, this will be made the primary interface by ARVRInterfaceGDNative
	}

	// and return our result
	return arvr_data->ovr->is_initialised();
}

////////////////////////////////////////////////////////////////
// Uninitialises our interface, shuts down our HMD
void godot_arvr_uninitialize(void *p_data) {
	arvr_data_struct *arvr_data = (arvr_data_struct *)p_data;

	// note, this will already be removed as the primary interface by
	// ARVRInterfaceGDNative

	arvr_data->ovr->cleanup();
}

////////////////////////////////////////////////////////////////
// Returns the requested size of our render target
// called right before rendering, if the size changes a new
// render target will be constructed.
godot_vector2 godot_arvr_get_render_targetsize(const void *p_data) {
	arvr_data_struct *arvr_data = (arvr_data_struct *)p_data;
	godot_vector2 size;

	if (arvr_data->ovr->is_initialised()) {
		// TODO: we should periodically check if the recommended size has changed (the user can adjust this) and if so update our width/height
		// and reset our render texture (RID)

		godot::api->godot_vector2_new(&size, (real_t)arvr_data->width, (real_t)arvr_data->height);
	} else {
		godot::api->godot_vector2_new(&size, 500.0f, 500.0f);
	}

	return size;
}

////////////////////////////////////////////////////////////////
// This is called while rendering to get each eyes view matrix
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
			godot::api->godot_vector3_new(&offset, -0.035f * world_scale, 0.0f, 0.0f);
		} else {
			godot::api->godot_vector3_new(&offset, 0.035f * world_scale, 0.0f, 0.0f);
		};
		godot::api->godot_transform_translated(&transform_for_eye, &offset);
	};

	// Now construct our full transform, the order may be in reverse, have to test
	// :)
	ret = *p_cam_transform;
	ret = godot::api->godot_transform_operator_multiply(&ret, &reference_frame);
	ret = godot::api->godot_transform_operator_multiply(&ret, arvr_data->ovr->get_hmd_transform());
	ret = godot::api->godot_transform_operator_multiply(&ret, &transform_for_eye);

	return ret;
}

////////////////////////////////////////////////////////////////
// This is called while rendering to get each eyes projection
// matrix
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

////////////////////////////////////////////////////////////////
// This is called after we render a frame for each eye so we
// can send the render output to OpenVR
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
			screen_rect.position.y = (0.5f * screen_rect.size.y) - (0.5f * new_height);
			screen_rect.size.y = new_height;
		} else {
			float new_width = screen_rect.size.y * (render_size->x / render_size->y);

			screen_rect.position.x = (0.5f * screen_rect.size.x) - (0.5f * new_width);
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

		vr::Texture_t eyeTexture = { (void *)(uintptr_t)texid, vr::TextureType_OpenGL, vr::ColorSpace_Auto };

		if (arvr_data->ovr->get_application_type() == openvr_data::OpenVRApplicationType::OVERLAY) {
			// Overlay mode
			if (p_eye == 1) {
				vr::EVROverlayError vrerr;

				for (int i = 0; i < arvr_data->ovr->get_overlay_count(); i++) {
					vr::TextureID_t texidov = (vr::TextureID_t)godot::VisualServer::get_singleton()->texture_get_texid(godot::VisualServer::get_singleton()->viewport_get_texture(arvr_data->ovr->get_overlay(i).viewport_rid));

					if (texid == texidov) {
						vrerr = vr::VROverlay()->SetOverlayTexture(arvr_data->ovr->get_overlay(i).handle, &eyeTexture);

						if (vrerr != vr::VROverlayError_None) {
							godot::Godot::print(godot::String("OpenVR could not set texture for overlay: ") + godot::String::num_int64(vrerr) + godot::String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));
						}

						vrerr = vr::VROverlay()->SetOverlayTextureBounds(arvr_data->ovr->get_overlay(i).handle, &bounds);

						if (vrerr != vr::VROverlayError_None) {
							godot::Godot::print(godot::String("OpenVR could not set textute bounds for overlay: ") + godot::String::num_int64(vrerr) + godot::String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));
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

////////////////////////////////////////////////////////////////
// Process is called by the rendering thread right before we
// render our next frame. Here we obtain our new poses.
// The HMD pose is used right away but tracker poses are used
// next frame.
void godot_arvr_process(void *p_data) {
	arvr_data_struct *arvr_data = (arvr_data_struct *)p_data;

	// this method gets called before every frame is rendered, here is where you
	// should update tracking data, update controllers, etc.
	if (arvr_data->ovr->is_initialised()) {
		// Call process on our ovr system.
		arvr_data->ovr->process();
	}
}

////////////////////////////////////////////////////////////////
// Construct our interface so it can be registered
// we do not initialise anything here!
void *godot_arvr_constructor(godot_object *p_instance) {
	// note, don't do to much here, not much will have been initialised yet...

	arvr_data_struct *arvr_data = (arvr_data_struct *)godot::api->godot_alloc(sizeof(arvr_data_struct));

	arvr_data->ovr = openvr_data::retain_singleton();
	arvr_data->video_driver = 0;

	return arvr_data;
}

////////////////////////////////////////////////////////////////
// Clean up our interface
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

////////////////////////////////////////////////////////////////
// Return a texture ID for the eye if we manage the final
// output buffer.
int godot_arvr_get_external_texture_for_eye(void *p_data, int p_eye) {
	return 0;
}

////////////////////////////////////////////////////////////////
// Receive notifications sent to our ARVROrigin node.
void godot_arvr_notification(void *p_data, int p_what) {
	// nothing to do here for now but we should implement this.
}

////////////////////////////////////////////////////////////////
// Return the camera feed that should be used for our background
// when we're dealing with AR.
int godot_arvr_get_camera_feed_id(void *) {
	return 0;
}

////////////////////////////////////////////////////////////////
// Return a texture ID for the eye if we manage the depth buffer
int godot_arvr_get_external_depth_for_eye(void *p_data, int p_eye) {
	return 0;
}

////////////////////////////////////////////////////////////////
// Structure to provide pointers to our interface functions.
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
	godot_arvr_notification,
	godot_arvr_get_camera_feed_id,
	// only available in Godot 3.3+
	godot_arvr_get_external_depth_for_eye
};
