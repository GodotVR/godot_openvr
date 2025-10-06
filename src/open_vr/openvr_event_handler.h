#ifndef OPENVR_EVENT_HANDLER_H
#define OPENVR_EVENT_HANDLER_H

#include <godot_cpp/classes/node.hpp>

#include "openvr_data.h"

namespace godot {
class OpenVREventHandler : public Node {
	GDCLASS(OpenVREventHandler, Node)

public:
	// Register an event type which exists in a newer version of OpenVR.
	void register_event_signal(uint32_t p_event_id, openvr_data::OpenVREventDataType p_type, String p_signal_name);

private:
	openvr_data *ovr;

protected:
	static void _bind_methods();

	OpenVREventHandler();
	~OpenVREventHandler();
};
} // namespace godot

#endif /* OPENVR_EVENT_HANDLER_H */
