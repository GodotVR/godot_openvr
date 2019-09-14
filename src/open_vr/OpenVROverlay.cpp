////////////////////////////////////////////////////////////////////////////////////////////////
// GDNative module that exposes overlay functions from OpenVR to Godot

// Note, even though this is pure C code, we're using the C++ compiler as
// Microsoft never updated their C compiler to understand more modern dialects
// and openvr uses pesky things such as namespaces

#include "OpenVROverlay.h"

using namespace godot;

void OpenVROverlay::_register_methods() {
	register_method("create_overlay", &OpenVROverlay::create_overlay);
	register_method("destroy_overlay", &OpenVROverlay::destroy_overlay);

	// I'm not sure if using properties here is safe, it may end up setting stuff before we're ready to set them...

	register_method("get_overlay_width_in_meters", &OpenVROverlay::get_overlay_width_in_meters);
	register_method("set_overlay_width_in_meters", &OpenVROverlay::set_overlay_width_in_meters);
	register_property<OpenVROverlay, real_t>("overlay_width", &OpenVROverlay::set_overlay_width_in_meters, &OpenVROverlay::get_overlay_width_in_meters, 1.0);

	register_method("is_overlay_visible", &OpenVROverlay::is_overlay_visible);
	register_method("set_overlay_visible", &OpenVROverlay::set_overlay_visible);
	register_method("show_overlay", &OpenVROverlay::show_overlay);
	register_method("hide_overlay", &OpenVROverlay::hide_overlay);
	register_property<OpenVROverlay, bool>("overlay_visible", &OpenVROverlay::set_overlay_visible, &OpenVROverlay::is_overlay_visible, false);

	register_method("track_relative_to_device", &OpenVROverlay::track_relative_to_device);
	register_method("overlay_position_absolute", &OpenVROverlay::overlay_position_absolute);
}

void OpenVROverlay::_init() {
	// nothing to do here
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

bool OpenVROverlay::create_overlay(String p_overlay_key, String p_overlay_name) {
	CharString overlay_key = p_overlay_key.ascii();
	CharString overlay_name = p_overlay_name.ascii();

	vr::VROverlayHandle_t overlay;
	vr::EVROverlayError vrerr = vr::VROverlay()->CreateOverlay(overlay_key.get_data(), overlay_name.get_data(), &overlay);
	if (vrerr != vr::VROverlayError_None) {
		Godot::print(String("Could not create overlay, OpenVR error:") + String::num_int64(vrerr) + ", " + String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));

		return false;
	}

	ovr->set_overlay(overlay);

	return true;
}


bool OpenVROverlay::destroy_overlay() {
	vr::EVROverlayError vrerr = vr::VROverlay()->DestroyOverlay(ovr->get_overlay());
	if (vrerr != vr::VROverlayError_None) {
		Godot::print(String("Could not destroy overlay, OpenVR error: ") + String::num_int64(vrerr) + ", " + String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));

		return false;
	}

	ovr->set_overlay(0);
	return true;
}

real_t OpenVROverlay::get_overlay_width_in_meters() const {
	float overlay_size;

	vr::VROverlay()->GetOverlayWidthInMeters(ovr->get_overlay(), &overlay_size);
	return overlay_size;
}

void OpenVROverlay::set_overlay_width_in_meters(real_t p_new_size) {
	vr::EVROverlayError vrerr = vr::VROverlay()->SetOverlayWidthInMeters(ovr->get_overlay(), p_new_size);

	if (vrerr != vr::VROverlayError_None) {
		Godot::print(String("Could not set overlay width in meters, OpenVR error: ") + String::num_int64(vrerr) + ", " + String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));
	}
}

bool OpenVROverlay::is_overlay_visible() const {
	return vr::VROverlay()->IsOverlayVisible(ovr->get_overlay());
}

void OpenVROverlay::set_overlay_visible(bool p_visible) {
	if (p_visible) {
		show_overlay();
	} else {
		hide_overlay();
	}
}

void OpenVROverlay::show_overlay() {
	vr::EVROverlayError vrerr = vr::VROverlay()->ShowOverlay(ovr->get_overlay());

	if (vrerr != vr::VROverlayError_None) {
		Godot::print(String("Could not show overlay, OpenVR error: ") + String::num_int64(vrerr) + ", " + String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));
	}
}

void OpenVROverlay::hide_overlay() {
	vr::EVROverlayError vrerr = vr::VROverlay()->HideOverlay(ovr->get_overlay());

	if (vrerr != vr::VROverlayError_None) {
		Godot::print(String("Could not hide overlay, OpenVR error: ") + String::num_int64(vrerr) + ", " + String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));
	}
}

bool OpenVROverlay::track_relative_to_device(vr::TrackedDeviceIndex_t p_tracked_device_index, Transform p_transform) {
	vr::HmdMatrix34_t matrix;
	
	ovr->matrix_from_transform(&matrix, (godot_transform *)&p_transform, godot::arvr_api->godot_arvr_get_worldscale());

	vr::EVROverlayError vrerr =  vr::VROverlay()->SetOverlayTransformTrackedDeviceRelative(ovr->get_overlay(), p_tracked_device_index, &matrix);

	if (vrerr != vr::VROverlayError_None) {
		Godot::print(String("Could not track overlay relative to device, OpenVR error: ") + String::num_int64(vrerr) + ", " + String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));

		return false;
	}

	return true;
}

bool OpenVROverlay::overlay_position_absolute(Transform p_transform) {
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

	vr::EVROverlayError vrerr =  vr::VROverlay()->SetOverlayTransformAbsolute(ovr->get_overlay(), origin, &matrix);

	if (vrerr != vr::VROverlayError_None) {
		Godot::print(String("Could not track overlay absolute, OpenVR error: ") + String::num_int64(vrerr) + ", " + String(vr::VROverlay()->GetOverlayErrorNameFromEnum(vrerr)));

		return false;
	}

	return true;
}

