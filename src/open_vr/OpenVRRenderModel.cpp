////////////////////////////////////////////////////////////////////////////////////////////////
// GDNative module that exposes the render models available in OpenVR as resources to Godot

#include "OpenVRRenderModel.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "godot_cpp/classes/image.hpp"
#include "godot_cpp/classes/image_texture.hpp"
#include "godot_cpp/classes/standard_material3d.hpp"

using namespace godot;

void OpenVRRenderModel::_bind_methods() {
	ClassDB::bind_method(D_METHOD("model_names"), &OpenVRRenderModel::model_names);
	ClassDB::bind_method(D_METHOD("load_model"), &OpenVRRenderModel::load_model);
}

OpenVRRenderModel::OpenVRRenderModel() {
	ovr = openvr_data::retain_singleton();
}

OpenVRRenderModel::~OpenVRRenderModel() {
	if (ovr != nullptr) {
		ovr->remove_mesh(this);
		ovr->release();
		ovr = nullptr;
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

	Array arr;
	arr.push_back(String(p_model_name));
	UtilityFunctions::print(String("Loading: {0}").format(arr));
	ovr->load_render_model(p_model_name, this);

	return success;
}
