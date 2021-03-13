#include "OpenVRFocus.h"

using namespace godot;

void OpenVRFocus::_register_methods() {
	register_method("_process", &OpenVRFocus::_process);

	register_method("get_is_dashboard_active", &OpenVRFocus::get_is_dashboard_active);

	register_property<OpenVRFocus, bool>("use_auto_pause", &OpenVRFocus::set_auto_pause_on_dashboard, &OpenVRFocus::get_auto_pause_on_dashboard, false);
	
	register_signal<OpenVRFocus>(String("dashboard_opened"), Dictionary());
	register_signal<OpenVRFocus>(String("dashboard_closed"), Dictionary());
}

void OpenVRFocus::_init() {
	dashboard_triggers_pause = false;
}

void OpenVRFocus::_process(float delta) {
    if (ovr != NULL) {
        bool is_dashboard_active_current = ovr->is_dashboard_active();
        if (is_dashboard_active != is_dashboard_active_current) {
            is_dashboard_active = is_dashboard_active_current;
            if (is_dashboard_active) {
                emit_signal("dashboard_opened");
				if (dashboard_triggers_pause) {
					get_tree()->set_pause(true);
				}
            } else {
				emit_signal("dashboard_closed");
				// Never auto un-pause as the player will want to confirm game status themselves first
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
	is_dashboard_active = false;
}

OpenVRFocus::~OpenVRFocus() {
	if (ovr != NULL) {
		ovr->release();
		ovr = NULL;
	}
}