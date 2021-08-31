////////////////////////////////////////////////////////////////////////////////////////////////
// GDNative module that exposes overlay functions from OpenVR to Godot

#include "OpenVROverlay.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void OpenVROverlay::_bind_methods() {
	// ClassDB::bind_method(D_METHOD("_ready"), &OpenVROverlay::_ready);
	// ClassDB::bind_method(D_METHOD("_exit_tree"), &OpenVROverlay::_exit_tree);

	ClassDB::bind_method(D_METHOD("is_overlay_visible"), &OpenVROverlay::is_overlay_visible);
	ClassDB::bind_method(D_METHOD("set_overlay_visible", "visible"), &OpenVROverlay::set_overlay_visible);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "overlay_visible"), "set_overlay_visible", "is_overlay_visible");

	ClassDB::bind_method(D_METHOD("get_overlay_width_in_meters"), &OpenVROverlay::get_overlay_width_in_meters);
	ClassDB::bind_method(D_METHOD("set_overlay_width_in_meters", "width"), &OpenVROverlay::set_overlay_width_in_meters);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "overlay_width_in_meters"), "set_overlay_width_in_meters", "get_overlay_width_in_meters");

	ClassDB::bind_method(D_METHOD("track_relative_to_device"), &OpenVROverlay::track_relative_to_device);
	ClassDB::bind_method(D_METHOD("overlay_position_absolute"), &OpenVROverlay::overlay_position_absolute);
}

OpenVROverlay::OpenVROverlay() {
	ovr = openvr_data::retain_singleton();
	overlay_width_in_meters = 1.0;
	overlay_visible = true;
	overlay = 0;
}

OpenVROverlay::~OpenVROverlay() {
	if (ovr != nullptr) {
		ovr->release();
		ovr = nullptr;
	}
}

void OpenVROverlay::_ready() {
	String appname = ProjectSettings::get_singleton()->get_setting("application/config/name");
	String overlay_identifier = appname + String::num(ovr->get_overlay_count() + 1);

	const char *overlay_key = overlay_identifier.utf8().get_data();
	const char *overlay_name = overlay_key;

	vr::EVROverlayError vrerr = vr::VROverlay()->CreateOverlay(overlay_key, overlay_name, &overlay);
	if (vrerr != vr::VROverlayError_None) {
		Array arr;
		arr.push_back(String::num(vrerr));
		arr.push_back(String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));
		UtilityFunctions::print(String("Could not create overlay, OpenVR error: {0}, {1}").format(arr));
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
			Array arr;
			arr.push_back(String::num(vrerr));
			arr.push_back(String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));
			UtilityFunctions::print(String("Could not destroy overlay, OpenVR error: {0}, {1}").format(arr));
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
			Array arr;
			arr.push_back(String::num(vrerr));
			arr.push_back(String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));
			UtilityFunctions::print(String("Could not set overlay width in meters, OpenVR error: {0}, {1}").format(arr));
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
				Array arr;
				arr.push_back(String::num(vrerr));
				arr.push_back(String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));
				UtilityFunctions::print(String("Could not show overlay, OpenVR error: {0}, {1}").format(arr));
			}
		} else {
			vr::EVROverlayError vrerr = vr::VROverlay()->HideOverlay(overlay);

			if (vrerr != vr::VROverlayError_None) {
				Array arr;
				arr.push_back(String::num(vrerr));
				arr.push_back(String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));
				UtilityFunctions::print(String("Could not hide overlay, OpenVR error: {0}, {1}").format(arr));
			}
		}
	}
}

bool OpenVROverlay::track_relative_to_device(vr::TrackedDeviceIndex_t p_tracked_device_index, Transform3D p_transform) {
	if (overlay) {
		XRServer *server = XRServer::get_singleton();
		double ws = server->get_world_scale();
		vr::HmdMatrix34_t matrix;

		ovr->matrix_from_transform(&matrix, &p_transform, ws);

		vr::EVROverlayError vrerr = vr::VROverlay()->SetOverlayTransformTrackedDeviceRelative(overlay, p_tracked_device_index, &matrix);

		if (vrerr != vr::VROverlayError_None) {
			Array arr;
			arr.push_back(String::num(vrerr));
			arr.push_back(String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));
			UtilityFunctions::print(String("Could not track overlay relative to device, OpenVR error: {0}, {1}").format(arr));

			return false;
		}

		return true;
	}
	return false;
}

bool OpenVROverlay::overlay_position_absolute(Transform3D p_transform) {
	if (overlay) {
		XRServer *server = XRServer::get_singleton();
		double ws = server->get_world_scale();
		vr::HmdMatrix34_t matrix;
		vr::TrackingUniverseOrigin origin;

		ovr->matrix_from_transform(&matrix, &p_transform, ws);

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
			Array arr;
			arr.push_back(String::num(vrerr));
			arr.push_back(String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));
			UtilityFunctions::print(String("Could not track overlay absolute, OpenVR error: {0}, {1}").format(arr));

			return false;
		}

		return true;
	}
	return false;
}
