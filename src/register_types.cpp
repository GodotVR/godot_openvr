////////////////////////////////////////////////////////////////////////////
// OpenVR GDExtension module for Godot
//
// Written by Bastiaan "Mux213" Olij,
// with loads of help from Thomas "Karroffel" Herzog

#include "register_types.h"

#include <gdextension_interface.h>

#include <godot_cpp/classes/xr_server.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "OpenVRRenderModel.h"
#include "OpenVRSkeleton.h"
#include "openvr_overlay_container.h"
#include "xr_interface_openvr.h"

using namespace godot;

// Due to godotengine/godot#64975, we can't add our interface to the XR
// server here. Instead, we register an autoload singleton ourselves in
// EditorPlugin.cpp.

void initialize_gdextension_types(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	ClassDB::register_class<XRInterfaceOpenVR>();
	ClassDB::register_class<OpenVROverlayContainer>();
	ClassDB::register_class<OpenVRRenderModel>();
	ClassDB::register_class<OpenVRSkeleton>();
}

extern "C" {
GDExtensionBool GDE_EXPORT openvr_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
	GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);
	init_obj.register_initializer(initialize_gdextension_types);
	init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

	return init_obj.init();
}
}
