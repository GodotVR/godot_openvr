////////////////////////////////////////////////////////////////////////////////////////////////
// GDNative module that exposes overlay functions from OpenVR to Godot

#include "OpenVROverlay.h"

using namespace godot;

void OpenVROverlay::_register_methods() {
	register_method("_ready", &OpenVROverlay::_ready);
	register_method("_exit_tree", &OpenVROverlay::_exit_tree);

	register_method("is_overlay_visible", &OpenVROverlay::is_overlay_visible);
	register_method("set_overlay_visible", &OpenVROverlay::set_overlay_visible);
	register_property<OpenVROverlay, bool>("overlay_visible", &OpenVROverlay::set_overlay_visible, &OpenVROverlay::is_overlay_visible, true);

	register_method("get_overlay_width_in_meters", &OpenVROverlay::get_overlay_width_in_meters);
	register_method("set_overlay_width_in_meters", &OpenVROverlay::set_overlay_width_in_meters);
	register_property<OpenVROverlay, real_t>("overlay_width_in_meters", &OpenVROverlay::set_overlay_width_in_meters, &OpenVROverlay::get_overlay_width_in_meters, 1.0);

	register_method("track_relative_to_device", &OpenVROverlay::track_relative_to_device);
	register_method("overlay_position_absolute", &OpenVROverlay::overlay_position_absolute);
}

OpenVROverlay::OpenVROverlay() {
	ovr = openvr_data::retain_singleton();
}

OpenVROverlay::~OpenVROverlay() {
	if (ovr != NULL) {
		ovr->release();
		ovr = NULL;
	}
}

void OpenVROverlay::_init() {
	overlay_width_in_meters = 1.0;
	overlay_visible = true;
	overlay = 0;
}

void OpenVROverlay::_ready() {
	String appname = ProjectSettings::get_singleton()->get_setting("application/config/name");
	String overlay_identifier = appname + String::num_int64(ovr->get_overlay_count() + 1);

	CharString overlay_key = overlay_identifier.ascii();
	CharString overlay_name = overlay_identifier.ascii();

	vr::EVROverlayError vrerr = vr::VROverlay()->CreateOverlay(overlay_key.get_data(), overlay_name.get_data(), &overlay);
	if (vrerr != vr::VROverlayError_None) {
		Godot::print(String("Could not create overlay, OpenVR error:") + String::num_int64(vrerr) + ", " + String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));
	}

	overlay_id = ovr->add_overlay(overlay, get_viewport_rid());

	Transform initial_transform;
	initial_transform = initial_transform.translated(Vector3(0, 0, 1) * -1.4);

	overlay_position_absolute(initial_transform);
	set_overlay_width_in_meters(overlay_width_in_meters);
	set_overlay_visible(overlay_visible);
	set_use_arvr(true);
}

void OpenVROverlay::_exit_tree() {
	if (overlay) {
		vr::EVROverlayError vrerr = vr::VROverlay()->DestroyOverlay(overlay);
		if (vrerr != vr::VROverlayError_None) {
			Godot::print(String("Could not destroy overlay, OpenVR error: ") + String::num_int64(vrerr) + ", " + String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));
		}

		overlay = 0;
		ovr->remove_overlay(overlay_id);
	}
}

real_t OpenVROverlay::get_overlay_width_in_meters() const {
	if (overlay) {
		float overlay_size;

		vr::VROverlay()->GetOverlayWidthInMeters(overlay, &overlay_size);
		return overlay_size;
	} else {
		return -1;
	}
}

void OpenVROverlay::set_overlay_width_in_meters(real_t p_new_size) {
	overlay_width_in_meters = p_new_size;

	if (overlay) {
		vr::EVROverlayError vrerr = vr::VROverlay()->SetOverlayWidthInMeters(overlay, p_new_size);

		if (vrerr != vr::VROverlayError_None) {
			Godot::print(String("Could not set overlay width in meters, OpenVR error: ") + String::num_int64(vrerr) + ", " + String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));
		}
	}
}

bool OpenVROverlay::is_overlay_visible() const {
	if (overlay) {
		return vr::VROverlay()->IsOverlayVisible(overlay);
	} else {
		return false;
	}
}

void OpenVROverlay::set_overlay_visible(bool p_visible) {
	overlay_visible = p_visible;

	if (overlay) {
		if (p_visible) {
			vr::EVROverlayError vrerr = vr::VROverlay()->ShowOverlay(overlay);

			if (vrerr != vr::VROverlayError_None) {
				Godot::print(String("Could not show overlay, OpenVR error: ") + String::num_int64(vrerr) + ", " + String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));
			}
		} else {
			vr::EVROverlayError vrerr = vr::VROverlay()->HideOverlay(overlay);

			if (vrerr != vr::VROverlayError_None) {
				Godot::print(String("Could not hide overlay, OpenVR error: ") + String::num_int64(vrerr) + ", " + String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));
			}
		}
	}
}

bool OpenVROverlay::track_relative_to_device(vr::TrackedDeviceIndex_t p_tracked_device_index, Transform p_transform) {
	if (overlay) {
		vr::HmdMatrix34_t matrix;

		ovr->matrix_from_transform(&matrix, (godot_transform *)&p_transform, godot::arvr_api->godot_arvr_get_worldscale());

		vr::EVROverlayError vrerr = vr::VROverlay()->SetOverlayTransformTrackedDeviceRelative(overlay, p_tracked_device_index, &matrix);

		if (vrerr != vr::VROverlayError_None) {
			Godot::print(String("Could not track overlay relative to device, OpenVR error: ") + String::num_int64(vrerr) + ", " + String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));

			return false;
		}

		return true;
	}
}

bool OpenVROverlay::overlay_position_absolute(Transform p_transform) {
	if (overlay) {
		vr::HmdMatrix34_t matrix;
		vr::TrackingUniverseOrigin origin;

		ovr->matrix_from_transform(&matrix, (godot_transform *)&p_transform, godot::arvr_api->godot_arvr_get_worldscale());

		openvr_data::OpenVRTrackingUniverse tracking_universe = ovr->get_tracking_universe();
		if (tracking_universe == openvr_data::OpenVRTrackingUniverse::SEATED) {
			origin = vr::TrackingUniverseSeated;
		} else if (tracking_universe == openvr_data::OpenVRTrackingUniverse::STANDING) {
			origin = vr::TrackingUniverseStanding;
		} else {
			origin = vr::TrackingUniverseRawAndUncalibrated;
		}

		vr::EVROverlayError vrerr = vr::VROverlay()->SetOverlayTransformAbsolute(overlay, origin, &matrix);

		if (vrerr != vr::VROverlayError_None) {
			Godot::print(String("Could not track overlay absolute, OpenVR error: ") + String::num_int64(vrerr) + ", " + String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));

			return false;
		}

		return true;
	}
}
