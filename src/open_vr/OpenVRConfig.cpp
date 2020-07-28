////////////////////////////////////////////////////////////////////////////////////////////////
// GDNative module that exposes some OpenVR module options to Godot

#include "OpenVRConfig.h"

using namespace godot;

void OpenVRConfig::_register_methods() {
	register_method("get_application_type", &OpenVRConfig::get_application_type);
	register_method("set_application_type", &OpenVRConfig::set_application_type);
	register_property<OpenVRConfig, int>("application_type", &OpenVRConfig::set_application_type, &OpenVRConfig::get_application_type, 0);

	register_method("get_tracking_universe", &OpenVRConfig::get_tracking_universe);
	register_method("set_tracking_universe", &OpenVRConfig::set_tracking_universe);
	register_property<OpenVRConfig, int>("tracking_universe", &OpenVRConfig::set_tracking_universe, &OpenVRConfig::get_tracking_universe, 0);

	register_method("get_action_json_path", &OpenVRConfig::get_action_json_path);
	register_method("set_action_json_path", &OpenVRConfig::set_action_json_path);
	register_property<OpenVRConfig, String>("action_json_path", &OpenVRConfig::set_action_json_path, &OpenVRConfig::get_action_json_path, String());

	register_method("get_default_action_set", &OpenVRConfig::get_default_action_set);
	register_method("set_default_action_set", &OpenVRConfig::set_default_action_set);
	register_property<OpenVRConfig, String>("default_action_set", &OpenVRConfig::set_default_action_set, &OpenVRConfig::get_default_action_set, String());

	register_method("register_action_set", &OpenVRConfig::register_action_set);
	register_method("set_active_action_set", &OpenVRConfig::set_active_action_set);
	register_method("toggle_action_set_active", &OpenVRConfig::toggle_action_set_active);
	register_method("is_action_set_active", &OpenVRConfig::is_action_set_active);

	register_method("play_area_available", &OpenVRConfig::play_area_available);
	register_method("get_play_area", &OpenVRConfig::get_play_area);
}

void OpenVRConfig::_init() {
	// nothing to do here
}

OpenVRConfig::OpenVRConfig() {
	ovr = openvr_data::retain_singleton();
	server = ARVRServer::get_singleton();
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

String OpenVRConfig::get_action_json_path() const {
	return String(ovr->get_action_json_path());
}

void OpenVRConfig::set_action_json_path(const String p_path) {
	ovr->set_action_json_path(p_path.utf8().get_data());
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

PoolVector3Array OpenVRConfig::get_play_area() const {
	const Vector3 *play_area = ovr->get_play_area();
	Transform reference = server->get_reference_frame();
	float ws = server->get_world_scale();

	PoolVector3Array arr;
	arr.resize(4);

	{
		PoolVector3Array::Write w = arr.write();

		for (int i = 0; i < 4; i++) {
			w[i] = reference.xform_inv(play_area[i]) * ws;
		}
	}

	return arr;
}
