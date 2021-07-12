////////////////////////////////////////////////////////////////////////////////////////////////
// GDNative module that exposes the render models available in OpenVR as resources to Godot

#include "OpenVRRenderModel.h"

#include <Array.hpp>
#include <String.hpp>
#include <Utilities.hpp>

// #include "SpatialMaterial.h"
// #include "ImageTexture.h"
// #include "Image.h"

using namespace godot;

void OpenVRRenderModel::_register_methods() {
	register_method("model_names", &OpenVRRenderModel::model_names);
	register_method("load_model", &OpenVRRenderModel::load_model);
}

void OpenVRRenderModel::_init() {
	// nothing to do here
}

OpenVRRenderModel::OpenVRRenderModel() {
	ovr = openvr_data::retain_singleton();
}

OpenVRRenderModel::~OpenVRRenderModel() {
	if (ovr != NULL) {
		ovr->remove_mesh(this);
		ovr->release();
		ovr = NULL;
	}
}

Array OpenVRRenderModel::model_names() {
	Array arr;

	if (ovr->is_initialised()) {
		int model_count = ovr->get_render_model_count();
		for (int m = 0; m < model_count; m++) {
			String s = ovr->get_render_model_name(m);

			arr.push_back(s);
		}
	}

	return arr;
}

bool OpenVRRenderModel::load_model(String p_model_name) {
	bool success = true;

	clear_surfaces();

	Utilities::print(String("Loading: {0}").format(Array::make(String(p_model_name))));
	ovr->load_render_model(p_model_name, this);

	return success;
}
