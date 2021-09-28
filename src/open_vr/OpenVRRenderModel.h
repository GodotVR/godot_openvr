////////////////////////////////////////////////////////////////////////////////////////////////
// GDNative module that exposes the render models available in OpenVR as resources to Godot

#ifndef OPENVR_RENDER_MODEL_H
#define OPENVR_RENDER_MODEL_H

#include "openvr_data.h"
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/godot.hpp>

namespace godot {
class OpenVRRenderModel : public ArrayMesh {
	GDCLASS(OpenVRRenderModel, ArrayMesh)

private:
	openvr_data *ovr;

protected:
	static void _bind_methods();

public:
	OpenVRRenderModel();
	~OpenVRRenderModel();

	Array model_names();
	bool load_model(String p_model_name);
};
} // namespace godot

#endif /* !OPENVR_RENDER_MODEL_H */
