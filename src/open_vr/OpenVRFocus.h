////////////////////////////////////////////////////////////////////////////////////////////////
// GDNative module that exposes some OpenVR focus events and status to Godot
//
// Written by James "Nidonocu" Shaw

#ifndef OPENVR_FOCUS_H
#define OPENVR_FOCUS_H

#include "openvr_data.h"
#include <Node.hpp>

namespace godot {

class OpenVRFocus : public Node {
	GODOT_CLASS(OpenVRFocus, Node)

private:
	openvr_data *ovr;

    bool is_dashboard_active;
	bool dashboard_triggers_pause;

protected:
public:
	static void _register_methods();

	void _init();
	void _process(float delta);

    OpenVRFocus();
	~OpenVRFocus();

    bool get_is_dashboard_active();
	bool get_auto_pause_on_dashboard();
	void set_auto_pause_on_dashboard(bool p_auto);
};

} // namespace godot

#endif /* !OPENVR_FOCUS_H */