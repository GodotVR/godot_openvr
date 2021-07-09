////////////////////////////////////////////////////////////////////////////////////////////////
// GDNative module that exposes the render models available in OpenVR as resources to Godot

#ifndef OPENVR_RENDER_MODEL_H
#define OPENVR_RENDER_MODEL_H

#include "openvr_data.h"
#include <ArrayMesh.hpp>
#include <Godot.hpp>

namespace godot {
class OpenVRRenderModel : public ArrayMesh {
	GODOT_CLASS(OpenVRRenderModel, ArrayMesh)

private:
	openvr_data *ovr;

public:
	static void _register_methods();

	void _init();

	OpenVRRenderModel();
	~OpenVRRenderModel();

	Array model_names();
	bool load_model(String p_model_name);
};
} // namespace godot

#endif /* !OPENVR_RENDER_MODEL_H */
