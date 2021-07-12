////////////////////////////////////////////////////////////////////////////////////////////////
// GDNative module that exposes some OpenVR module options to Godot

#include "OpenVRConfig.h"
#include "Utilities.hpp"

using namespace godot;

void OpenVRConfig::_register_methods() {
	register_method("get_application_type", &OpenVRConfig::get_application_type);
	register_method("set_application_type", &OpenVRConfig::set_application_type);
	register_property<OpenVRConfig, int>("application_type", &OpenVRConfig::set_application_type, &OpenVRConfig::get_application_type, 0);

	register_method("get_tracking_universe", &OpenVRConfig::get_tracking_universe);
	register_method("set_tracking_universe", &OpenVRConfig::set_tracking_universe);
	register_property<OpenVRConfig, int>("tracking_universe", &OpenVRConfig::set_tracking_universe, &OpenVRConfig::get_tracking_universe, 0);

	register_method("get_default_action_set", &OpenVRConfig::get_default_action_set);
	register_method("set_default_action_set", &OpenVRConfig::set_default_action_set);
	register_property<OpenVRConfig, String>("default_action_set", &OpenVRConfig::set_default_action_set, &OpenVRConfig::get_default_action_set, String());

	register_method("register_action_set", &OpenVRConfig::register_action_set);
	register_method("set_active_action_set", &OpenVRConfig::set_active_action_set);
	register_method("toggle_action_set_active", &OpenVRConfig::toggle_action_set_active);
	register_method("is_action_set_active", &OpenVRConfig::is_action_set_active);

	register_method("play_area_available", &OpenVRConfig::play_area_available);
	register_method("get_play_area", &OpenVRConfig::get_play_area);

	register_method("get_device_battery_percentage", &OpenVRConfig::get_device_battery_percentage);
	register_method("is_device_charging", &OpenVRConfig::is_device_charging);
}

void OpenVRConfig::_init() {
	// nothing to do here
}

OpenVRConfig::OpenVRConfig() {
	ovr = openvr_data::retain_singleton();
	server = XRServer::get_singleton();
}

OpenVRConfig::~OpenVRConfig() {
	if (ovr != NULL) {
		ovr->release();
		ovr = NULL;
	}
}

int OpenVRConfig::get_application_type() const {
	return ovr->get_application_type();
}

void OpenVRConfig::set_application_type(int p_type) {
	ovr->set_application_type((openvr_data::OpenVRApplicationType)p_type);
}

int OpenVRConfig::get_tracking_universe() const {
	return ovr->get_tracking_universe();
}

void OpenVRConfig::set_tracking_universe(int p_universe) {
	ovr->set_tracking_universe((openvr_data::OpenVRTrackingUniverse)p_universe);
}

String OpenVRConfig::get_default_action_set() const {
	return ovr->get_default_action_set();
}

void OpenVRConfig::set_default_action_set(const String p_name) {
	ovr->set_default_action_set(p_name);
}

void OpenVRConfig::register_action_set(const String p_action_set) {
	ovr->register_action_set(p_action_set);
}

void OpenVRConfig::set_active_action_set(const String p_action_set) {
	ovr->set_active_action_set(p_action_set);
}

void OpenVRConfig::toggle_action_set_active(const String p_action_set, const bool p_is_active) {
	ovr->toggle_action_set_active(p_action_set, p_is_active);
}

bool OpenVRConfig::is_action_set_active(const String p_action_set) const {
	return ovr->is_action_set_active(p_action_set);
}

bool OpenVRConfig::play_area_available() const {
	return ovr->play_area_available();
}

PackedVector3Array OpenVRConfig::get_play_area() const {
	const Vector3 *play_area = ovr->get_play_area();
	Transform3D reference = server->get_reference_frame();
	float ws = server->get_world_scale();

	PackedVector3Array arr;
	arr.resize(4);

	for (int i = 0; i < 4; i++) {
		arr[i] = reference.xform_inv(play_area[i]) * ws;
	}

	return arr;
}

float OpenVRConfig::get_device_battery_percentage(vr::TrackedDeviceIndex_t p_tracked_device_index) {
	vr::ETrackedPropertyError pError;
	float battery_percentage = ovr->hmd->GetFloatTrackedDeviceProperty(p_tracked_device_index, vr::Prop_DeviceBatteryPercentage_Float, &pError);

	if (pError != vr::TrackedProp_Success) {
		Utilities::print(String("Could not get battery percentage, OpenVR error: {0}, {1} ").format(Array::make(Variant(pError), String(ovr->hmd->GetPropErrorNameFromEnum(pError)))));
	}

	return battery_percentage;
}

bool OpenVRConfig::is_device_charging(vr::TrackedDeviceIndex_t p_tracked_device_index) {
	vr::ETrackedPropertyError pError;
	bool is_charging = ovr->hmd->GetBoolTrackedDeviceProperty(p_tracked_device_index, vr::Prop_DeviceIsCharging_Bool, &pError);

	if (pError != vr::TrackedProp_Success) {
		Utilities::print(String("Could not get charging state, OpenVR error: {0}, {1} ").format(Array::make(Variant(pError), String(ovr->hmd->GetPropErrorNameFromEnum(pError)))));
	}

	return is_charging;
}
