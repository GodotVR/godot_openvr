#ifndef OPENVR_EVENT_SIGNALS_H
#define OPENVR_EVENT_SIGNALS_H

// This macro is used to define signals for each VREvent to make it impossible for the list of signals to get out of sync with
// the events. The name of the signal is automatically derived from the EVREventType. This would normally be impossible since
// there is no way to introspect the name of enum members. While IVRSystem provides GetEventTypeNameFromEnum, this would
// require connecting to OpenVR before creating our signals which makes the experience in the editor less than ideal.
#define VREVENT_SIGNAL(vrevent_id, vrevent_type, source)                             \
	{                                                                                \
		String name = String(#vrevent_id).trim_prefix("vr::EVREventType::VREvent_"); \
		ADD_SIGNAL(MethodInfo(name,                                                  \
				PropertyInfo(Variant::INT, "eventAgeSeconds"),                       \
				PropertyInfo(Variant::OBJECT, "positionalTracker"),                  \
				PropertyInfo(Variant::DICTIONARY, "data")));                         \
		openvr_data::register_event_signal(vrevent_id, vrevent_type, name);          \
	}

#endif /* OPENVR_EVENT_SIGNALS_H */
