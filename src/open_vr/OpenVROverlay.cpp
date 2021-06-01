////////////////////////////////////////////////////////////////////////////////////////////////
// GDNative module that exposes overlay functions from OpenVR to Godot

#include "OpenVROverlay.h"
#include "Utilities.hpp"

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
	String overlay_identifier = appname + String((int64_t)ovr->get_overlay_count() + 1);

	const char *overlay_key = overlay_identifier;
	const char *overlay_name = overlay_identifier;

	vr::EVROverlayError vrerr = vr::VROverlay()->CreateOverlay(overlay_key, overlay_name, &overlay);
	if (vrerr != vr::VROverlayError_None) {
		Utilities::print(String("Could not create overlay, OpenVR error:") + String((int64_t)vrerr) + ", " + String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));
	}

	overlay_id = ovr->add_overlay(overlay, get_viewport_rid());

	Transform3D initial_transform;
	initial_transform = initial_transform.translated(Vector3(0, 0, 1) * -1.4);

	overlay_position_absolute(initial_transform);
	set_overlay_width_in_meters(overlay_width_in_meters);
	set_overlay_visible(overlay_visible);
	set_use_xr(true);
}

void OpenVROverlay::_exit_tree() {
	if (overlay) {
		vr::EVROverlayError vrerr = vr::VROverlay()->DestroyOverlay(overlay);
		if (vrerr != vr::VROverlayError_None) {
			Utilities::print(String("Could not destroy overlay, OpenVR error: ") + String((int64_t)vrerr) + ", " + String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));
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
			Utilities::print(String("Could not set overlay width in meters, OpenVR error: ") + String((int64_t)vrerr) + ", " + String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));
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
				Utilities::print(String("Could not show overlay, OpenVR error: ") + String((int64_t)vrerr) + ", " + String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));
			}
		} else {
			vr::EVROverlayError vrerr = vr::VROverlay()->HideOverlay(overlay);

			if (vrerr != vr::VROverlayError_None) {
				Utilities::print(String("Could not hide overlay, OpenVR error: ") + String((int64_t)vrerr) + ", " + String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));
			}
		}
	}
}

bool OpenVROverlay::track_relative_to_device(vr::TrackedDeviceIndex_t p_tracked_device_index, Transform3D p_transform) {
	if (overlay) {
		vr::HmdMatrix34_t matrix;

		ovr->matrix_from_transform(&matrix, (godot_transform3d *)&p_transform, godot::xr_api->godot_xr_get_worldscale());

		vr::EVROverlayError vrerr = vr::VROverlay()->SetOverlayTransformTrackedDeviceRelative(overlay, p_tracked_device_index, &matrix);

		if (vrerr != vr::VROverlayError_None) {
			Utilities::print(String("Could not track overlay relative to device, OpenVR error: ") + String((int64_t)vrerr) + ", " + String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));

			return false;
		}

		return true;
	}
	return false;
}

bool OpenVROverlay::overlay_position_absolute(Transform3D p_transform) {
	if (overlay) {
		vr::HmdMatrix34_t matrix;
		vr::TrackingUniverseOrigin origin;

		ovr->matrix_from_transform(&matrix, (godot_transform3d *)&p_transform, godot::xr_api->godot_xr_get_worldscale());

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
			Utilities::print(String("Could not track overlay absolute, OpenVR error: ") + String((int64_t)vrerr) + ", " + String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));

			return false;
		}

		return true;
	}
	return false;
}
