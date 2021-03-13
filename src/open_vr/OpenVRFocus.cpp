#include "OpenVRFocus.h"

using namespace godot;

void OpenVRFocus::_register_methods() {
	register_method("_process", &OpenVRFocus::_process);

	register_method("get_is_dashboard_active", &OpenVRFocus::get_is_dashboard_active);

	register_property<OpenVRFocus, bool>("use_auto_pause", &OpenVRFocus::set_auto_pause_on_dashboard, &OpenVRFocus::get_auto_pause_on_dashboard, bool);
	
	register_signal<OpenVRFocus>(String("dashboard_opened"));
	register_signal<OpenVRFocus>(String("dashboard_closed"));
}

void OpenVRFocus::_init() {
}

void OpenVRFocus::_process(float delta) {
    if (ovr != NULL) {
        bool is_dashboard_active_current = ovr->is_dashboard_active
        if (is_dashboard_active != is_dashboard_active_current) {
            is_dashboard_active = is_dashboard_active_current;
            if (is_action_set_active) {
                emit_signal("dashboard_opened");
				if (dashboard_triggers_pause) {
					get_tree().paused = true;
				}
            } else {
				emit_signal("dashboard_closed");
            }
        }
    }
}

bool OpenVRFocus::get_is_dashboard_active() {
    return is_dashboard_active;
}

bool OpenVRFocus::get_auto_pause_on_dashboard() {
    return dashboard_triggers_pause;
}

void OpenVRFocus::set_auto_pause_on_dashboard(bool p_auto) {
    dashboard_triggers_pause = p_auto;
}

OpenVRFocus::OpenVRFocus() {
	ovr = openvr_data::retain_singleton();
}

OpenVRFocus::~OpenVRFocus() {
	if (ovr != NULL) {
		ovr->release();
		ovr = NULL;
	}
}