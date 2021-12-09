////////////////////////////////////////////////////////////////////////////
// OpenVR GDExtension module for Godot
//
// Written by Bastiaan "Mux213" Olij,
// with loads of help from Thomas "Karroffel" Herzog

#include "register_types.h"

#include <godot/gdnative_interface.h>

#include <godot_cpp/classes/xr_server.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "OpenVROverlay.h"
#include "OpenVRRenderModel.h"
#include "OpenVRSkeleton.h"
#include "xr_interface_openvr.h"

using namespace godot;

Ref<XRInterfaceOpenVR> xr_interface_openvr;

void register_types() {
	// UtilityFunctions::print("Hello register types!");

	ClassDB::register_class<XRInterfaceOpenVR>();
	ClassDB::register_class<OpenVROverlay>();
	ClassDB::register_class<OpenVRRenderModel>();
	ClassDB::register_class<OpenVRSkeleton>();

	XRServer *xr_server = XRServer::get_singleton();
	ERR_FAIL_NULL(xr_server);

	xr_interface_openvr.instantiate();
	xr_server->add_interface(xr_interface_openvr);
}

void unregister_types() {
	if (xr_interface_openvr.is_valid()) {
		if (xr_interface_openvr->is_initialized()) {
			xr_interface_openvr->uninitialize();
		}

		XRServer *xr_server = XRServer::get_singleton();
		ERR_FAIL_NULL(xr_server);
		xr_server->remove_interface(xr_interface_openvr);

		xr_interface_openvr.unref();
	}

	// Note: our class will be unregistered automatically
}

extern "C" {
// Initialization.
GDNativeBool GDN_EXPORT openvr_library_init(const GDNativeInterface *p_interface, const GDNativeExtensionClassLibraryPtr p_library, GDNativeInitialization *r_initialization) {
	godot::GDExtensionBinding::InitObject init_obj(p_interface, p_library, r_initialization);

	init_obj.register_driver_initializer(register_types);
	init_obj.register_driver_terminator(unregister_types);

	return init_obj.init();
}
}
