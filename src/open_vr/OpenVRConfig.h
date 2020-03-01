////////////////////////////////////////////////////////////////////////////////////////////////
// GDNative module that exposes some OpenVR module options to Godot

#ifndef OPENVR_CONFIG_H
#define OPENVR_CONFIG_H

#include "openvr_data.h"
#include <Reference.hpp>
#include <String.hpp>

namespace godot {

class OpenVRConfig : public Reference {
	GODOT_CLASS(OpenVRConfig, Reference)

private:
	openvr_data *ovr;

public:
	static void _register_methods();

	void _init();

	OpenVRConfig();
	~OpenVRConfig();

	int get_application_type() const;
	void set_application_type(int p_type);

	int get_tracking_universe() const;
	void set_tracking_universe(int p_universe);

	String get_action_json_path() const;
	void set_action_json_path(const String p_path);

	String get_default_action_set() const;
	void set_default_action_set(const String p_name);

	void register_action_set(const String p_action_set);
	void set_active_action_set(const String p_action_set);
};

} // namespace godot

#endif /* !OPENVR_CONFIG_H */
